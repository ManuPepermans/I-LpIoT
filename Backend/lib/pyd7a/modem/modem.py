#!/usr/bin/env python
import time

from datetime import datetime

import struct
from threading import Thread

import serial
from bitstring import ConstBitStream

from d7a.alp.operands.file import DataRequest
from d7a.alp.operands.file import Offset
from d7a.alp.operations.requests import ReadFileData
from d7a.alp.operations.responses import ReturnFileData
from d7a.alp.regular_action import RegularAction
from d7a.serial_console_interface.parser import Parser

from d7a.alp.command import Command
from d7a.system_files.firmware_version import FirmwareVersionFile
from d7a.system_files.uid import UidFile

from d7a.system_files.system_file_ids import SystemFileIds

class Modem:
  def __init__(self, device, baudrate, unsolicited_response_received_callback, show_logging=True):
    self.show_logging = show_logging
    self.parser = Parser()
    self.config = {
      "device"   : device,
      "baudrate" : baudrate
    }

    self.uid = None
    self.firmware_version = None
    self._sync_execution_response_cmds = []
    self._sync_execution_tag_id = None
    self._sync_execution_completed = False
    self._unsolicited_responses_received = []
    self._read_async_active = False
    self.unsolicited_response_received_callback = unsolicited_response_received_callback

    connected = self._connect_serial_modem()
    if connected:
      print("connected to {}, node UID {} running D7AP v{}, application \"{}\" with git sha1 {}".format(
        self.config["device"], self.uid, self.firmware_version.d7ap_version,
        self.firmware_version.application_name, self.firmware_version.git_sha1)
      )
    else:
      raise ModemConnectionError


  def _connect_serial_modem(self):
    self.dev = serial.Serial(
      port     = self.config["device"],
      baudrate = self.config["baudrate"],
      timeout  = None,
    )

    self.dev.flush() # ignore possible buffered data
    self.start_reading()
    read_modem_info_action = Command.create_with_read_file_action_system_file(UidFile())
    read_modem_info_action.add_action(
      RegularAction(
        operation=ReadFileData(
          operand=DataRequest(
            offset=Offset(id=FirmwareVersionFile().id, offset=0),  # TODO offset size
            length=FirmwareVersionFile().length
          )
        )
      )
    )

    resp_cmd = self.execute_command(read_modem_info_action, timeout_seconds=60)

    if len(resp_cmd) == 0:
      self._log("Timed out reading node information")
      return False

    for action in resp_cmd[0].actions:
      if type(action) is RegularAction and type(action.operation) is ReturnFileData:
          if action.operand.offset.id == SystemFileIds.UID.value:
            self.uid = '{:x}'.format(struct.unpack(">Q", bytearray(action.operand.data))[0])
          if action.operand.offset.id == SystemFileIds.FIRMWARE_VERSION.value:
            self.firmware_version = FirmwareVersionFile.parse(ConstBitStream(bytearray(action.operand.data)))

    if self.uid and self.firmware_version:
      return True

    return False


  def _log(self, *msg):
    if self.show_logging: print " ".join(map(str, msg))

  def execute_command_async(self, alp_command):
    self.execute_command(alp_command, timeout_seconds=0)

  def execute_command(self, alp_command, timeout_seconds=10):
    data = self.parser.build_serial_frame(alp_command)
    self._sync_execution_response_cmds = []
    self._sync_execution_tag_id = None
    self._sync_execution_completed = False
    if(timeout_seconds > 0):
      assert self._sync_execution_tag_id is None
      self._sync_execution_tag_id = alp_command.tag_id

    self.dev.write(data)
    self.dev.flush()
    self._log("Sending command of size ", len(data))
    self._log("> " + " ".join(map(lambda b: format(b, "02x"), data)))
    if timeout_seconds == 0:
      return []

    self._log("Waiting for response (max {} s)".format(timeout_seconds))
    start_time = datetime.now()
    while not self._sync_execution_completed and (datetime.now() - start_time).total_seconds() < timeout_seconds:
      time.sleep(0.05)

    if not self._sync_execution_completed:
      self._log("Command timeout (tag {})".format(alp_command.tag_id))
      return []

    return self._sync_execution_response_cmds


  def start_reading(self):
    self._read_async_active = True
    self.read_thread = Thread(target=self._read_async)
    self.read_thread.daemon = True
    self.read_thread.start()


  def stop_reading(self):
    self._read_async_active = False
    self.dev.cancel_read()
    self.read_thread.join()

  def get_unsolicited_responses_received(self):
    return self._unsolicited_responses_received

  def clear_unsolicited_responses_received(self):
    self._unsolicited_responses_received = []

  def _read_async(self):
    self._log("starting read thread")
    data_received = bytearray()
    while self._read_async_active:
      try:
        data_received = self.dev.read()
      except serial.SerialException:
        self._log("SerialException received, trying to reconnect")
        self.dev.close()
        time.sleep(5)
        self._connect_serial_modem()

      if len(data_received) > 0:
        self._log("< " + " ".join(map(lambda b: format(b, "02x"), bytearray(data_received))))
        (cmds, info) = self.parser.parse(data_received)
        for error in info["errors"]:
          error["buffer"] = " ".join(map(lambda b: format(b, "02x"), bytearray(data_received)))
          self._log("Parser error: {}".format(error))

        for cmd in cmds:
          if self._sync_execution_tag_id == cmd.tag_id:
            self._log("Received response for sync execution")
            self._sync_execution_response_cmds.append(cmd)
            if cmd.execution_completed:
              self._log("cmd with tag {} done".format(cmd.tag_id))
              self._sync_execution_completed = True
            else:
              self._log("cmd with tag {} not done yet, expecting more responses".format(cmd.tag_id))

          elif self.unsolicited_response_received_callback != None:
            self.unsolicited_response_received_callback(cmd)
          else:
            self._log("Received a response which was not requested synchronously or no async callback provided")
            self._unsolicited_responses_received.append(cmd)

    self._log("end read thread")


class ModemConnectionError(Exception):
  pass
