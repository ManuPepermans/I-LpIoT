#!/usr/bin/env python

import argparse
import os
from time import sleep

from d7a.alp.command import Command
from d7a.alp.interface import InterfaceType
from d7a.d7anp.addressee import Addressee, IdType
from d7a.sp.configuration import Configuration
from d7a.sp.qos import QoS, ResponseMode
from d7a.system_files.uid import UidFile
from modem.modem import Modem

# This example can be used with a node running the gateway app included in OSS-7, which is connect using the supplied serial device.
# It will query the sensor file (file 0x40) from other nodes running sensor_pull, using adhoc synchronization and print the results.

def received_command_callback(cmd):
  print cmd
  if cmd.execution_completed:
    os._exit(0)

argparser = argparse.ArgumentParser()
argparser.add_argument("-d", "--device", help="serial device /dev file modem",
                            default="/dev/ttyUSB0")
argparser.add_argument("-r", "--rate", help="baudrate for serial device", type=int, default=115200)
argparser.add_argument("-v", "--verbose", help="verbose", default=False, action="store_true")
config = argparser.parse_args()

modem = Modem(config.device, config.rate, unsolicited_response_received_callback=received_command_callback, show_logging=config.verbose)
print "Executing query..."
modem.execute_command_async(
  alp_command=Command.create_with_read_file_action(
    file_id=0x40,
    length=8,
    interface_type=InterfaceType.D7ASP,
    interface_configuration=Configuration(
      qos=QoS(resp_mod=ResponseMode.RESP_MODE_ALL),
      addressee=Addressee(
        access_class=0x11,
        id_type=IdType.NOID
      )
    )
  )
)

while True:
  sleep(5)
