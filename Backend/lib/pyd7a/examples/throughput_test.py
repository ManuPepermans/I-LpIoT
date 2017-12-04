import argparse
import time

import sys
from collections import defaultdict

from d7a.alp.command import Command
from d7a.alp.interface import InterfaceType
from d7a.alp.operations.status import InterfaceStatus
from d7a.d7anp.addressee import Addressee, IdType
from d7a.dll.access_profile import AccessProfile, CsmaCaMode
from d7a.dll.sub_profile import SubProfile
from d7a.phy.channel_header import ChannelHeader, ChannelBand, ChannelClass, ChannelCoding
from d7a.phy.subband import SubBand
from d7a.sp.configuration import Configuration
from d7a.sp.qos import QoS, ResponseMode
from d7a.system_files.access_profile import AccessProfileFile
from d7a.types.ct import CT
from modem.modem import Modem
from d7a.alp.operations.responses import ReturnFileData
from d7a.system_files.dll_config import DllConfigFile


class ThroughtPutTest:
  def __init__(self):
    self.argparser = argparse.ArgumentParser(
      fromfile_prefix_chars="@",
      description="Test throughput over 2 serial D7 modems"
    )

    self.argparser.add_argument("-n", "--msg-count", help="number of messages to transmit", type=int, default=10)
    self.argparser.add_argument("-p", "--payload-size", help="number of bytes of (appl level) payload to transmit", type=int, default=50)
    self.argparser.add_argument("-sw", "--serial-transmitter", help="serial device /dev file transmitter node", default=None)
    self.argparser.add_argument("-sr", "--serial-receiver", help="serial device /dev file receiver node", default=None)
    self.argparser.add_argument("-r", "--rate", help="baudrate for serial device", type=int, default=115200)
    self.argparser.add_argument("-uid", "--unicast-uid", help="UID to use for unicast transmission, "
                                                              "when not using receiver "
                                                              "(in hexstring, for example 0xb57000009151d)", default=None)
    self.argparser.add_argument("-to", "--receiver-timeout", help="timeout for the receiver (in seconds)", type=int, default=10)
    self.argparser.add_argument("-v", "--verbose", help="verbose", default=False, action="store_true")
    self.config = self.argparser.parse_args()

    if self.config.serial_transmitter == None and self.config.serial_receiver == None:
      self.argparser.error("At least a transmitter or receiver is required.")

    if self.config.serial_receiver == None and self.config.unicast_uid == None:
      self.argparser.error("When running without receiver a --unicast-uid parameter is required.")

    if self.config.serial_transmitter == None:
      self.transmitter_modem = None
      print("Running without transmitter")
    else:
      self.transmitter_modem = Modem(self.config.serial_transmitter, self.config.rate, None, show_logging=self.config.verbose)
      access_profile = AccessProfile(
        channel_header=ChannelHeader(channel_band=ChannelBand.BAND_868,
                                     channel_coding=ChannelCoding.PN9,
                                     channel_class=ChannelClass.NORMAL_RATE),
        sub_profiles=[SubProfile(subband_bitmap=0x01, scan_automation_period=CT(exp=0, mant=0)), SubProfile(), SubProfile(), SubProfile()],
        sub_bands=[SubBand(
          channel_index_start=0,
          channel_index_end=0,
          eirp=10,
          cca=86 # TODO
        )]
      )

      print("Write Access Profile")
      write_ap_cmd = Command.create_with_write_file_action_system_file(file=AccessProfileFile(access_profile=access_profile, access_specifier=0))
      self.transmitter_modem.execute_command(write_ap_cmd, timeout_seconds=1)

    if self.config.serial_receiver == None:
      self.receiver_modem = None
      print("Running without receiver")
    else:
      self.receiver_modem = Modem(self.config.serial_receiver, self.config.rate, self.receiver_cmd_callback, show_logging=self.config.verbose)
      self.receiver_modem.execute_command(Command.create_with_write_file_action_system_file(DllConfigFile(active_access_class=0x01)), timeout_seconds=1)
      print("Receiver scanning on Access Class = 0x01")



  def start(self):
    self.received_commands = defaultdict(list)
    payload = range(self.config.payload_size)

    if self.receiver_modem != None:
      addressee_id = int(self.receiver_modem.uid, 16)
    else:
      addressee_id = int(self.config.unicast_uid, 16)

    if self.transmitter_modem != None:

      print("\n==> broadcast, with QoS, transmitter active access class = 0x01 ====")
      self.transmitter_modem.execute_command(Command.create_with_write_file_action_system_file(DllConfigFile(active_access_class=0x01)), timeout_seconds=1)
      interface_configuration = Configuration(
        qos=QoS(resp_mod=ResponseMode.RESP_MODE_ANY),
        addressee=Addressee(
          access_class=0x01,
          id_type=IdType.NBID,
          id=CT(exp=0, mant=1) # we expect one responder
        )
      )

      self.start_transmitting(interface_configuration=interface_configuration, payload=payload)
      self.wait_for_receiver(payload)

      print("\n==> broadcast, no QoS, transmitter active access class = 0x01 ====")
      self.transmitter_modem.execute_command(Command.create_with_write_file_action_system_file(DllConfigFile(active_access_class=0x01)), timeout_seconds=1)
      interface_configuration = Configuration(
        qos=QoS(resp_mod=ResponseMode.RESP_MODE_NO),
        addressee=Addressee(
          access_class=0x01,
          id_type=IdType.NOID
        )
      )

      self.start_transmitting(interface_configuration=interface_configuration, payload=payload)
      self.wait_for_receiver(payload)

      print("\n==> unicast, with QoS, transmitter active access class = 0x01")
      interface_configuration = Configuration(
        qos=QoS(resp_mod=ResponseMode.RESP_MODE_ANY),
        addressee=Addressee(
          access_class=0x01,
          id_type=IdType.UID,
          id=addressee_id
        )
      )

      self.start_transmitting(interface_configuration=interface_configuration, payload=payload)
      self.wait_for_receiver(payload)

      print("\n==> unicast, no QoS, transmitter active access class = 0x01")
      interface_configuration = Configuration(
        qos=QoS(resp_mod=ResponseMode.RESP_MODE_NO),
        addressee=Addressee(
          access_class=0x01,
          id_type=IdType.UID,
          id=addressee_id
        )
      )

      self.start_transmitting(interface_configuration=interface_configuration, payload=payload)
      self.wait_for_receiver(payload)
    else:
      # receive only
      self.wait_for_receiver(payload)

  def start_transmitting(self, interface_configuration, payload):
    print("Running throughput test with payload size {} and interface_configuration {}\n\nrunning ...\n".format(len(payload), interface_configuration))

    if self.receiver_modem != None:
      self.received_commands = defaultdict(list)

    command = Command.create_with_return_file_data_action(
      file_id=0x40,
      data=payload,
      interface_type=InterfaceType.D7ASP,
      interface_configuration=interface_configuration
    )

    start = time.time()

    for i in range(self.config.msg_count):
      sys.stdout.write("{}/{}\r".format(i + 1, self.config.msg_count))
      sys.stdout.flush()
      self.transmitter_modem.execute_command(command, timeout_seconds=100)

    end = time.time()
    print("transmitter: sending {} messages completed in: {} s".format(self.config.msg_count, end - start))
    print("transmitter: throughput = {} bps with a payload size of {} bytes".format(
      (self.config.msg_count * self.config.payload_size * 8) / (end - start), self.config.payload_size)
    )

  def wait_for_receiver(self, payload):
    if self.receiver_modem == None:
      print("Running without receiver so we are not waiting for messages to be received ...")
    else:
      start = time.time()
      total_recv = 0
      while total_recv < self.config.msg_count and time.time() - start < self.config.receiver_timeout:
        total_recv = sum(len(v) for v in self.received_commands.values())
        time.sleep(2)
        print("waiting for receiver to finish ... (current nr of recv msgs: {})".format(total_recv))

      print("finished receiving or timeout")
      payload_has_errors = False
      for sender_cmd in self.received_commands.values():
        for cmd in sender_cmd:
          if type(cmd.actions[0].op) != ReturnFileData and cmd.actions[0].operand.data != payload:
            payload_has_errors = True
            print ("receiver: received unexpected command: {}".format(cmd))

      if payload_has_errors == False and total_recv == self.config.msg_count:
        print("receiver: OK: received {} messages with correct payload:".format(total_recv))
        for sender, cmds in self.received_commands.items():
          print("\t{}: {}".format(sender, len(cmds)))
      else:
        print("receiver: NOK: received messages {}:".format(total_recv))
        for sender, cmds in self.received_commands.items():
          print("\t{}: {}".format(sender, len(cmds)))

  def receiver_cmd_callback(self, cmd):
    print("recv cmd: ".format(cmd))
    if cmd.interface_status != None:
      uid = cmd.interface_status.operand.interface_status.addressee.id
      self.received_commands[uid].append(cmd)
    else:
      print("Unexpected cmd received, reboot?\n\t{}".format(cmd))


if __name__ == "__main__":
  ThroughtPutTest().start()
