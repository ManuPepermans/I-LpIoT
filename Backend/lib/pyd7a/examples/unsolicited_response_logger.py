#!/usr/bin/env python

import argparse

from d7a.alp.command import Command
from d7a.alp.interface import InterfaceType
from d7a.d7anp.addressee import Addressee, IdType
from d7a.sp.configuration import Configuration
from d7a.sp.qos import QoS, ResponseMode
from d7a.system_files.uid import UidFile
from modem.modem import Modem


# This example can be used with a node running the gateway app included in OSS-7.
# The gateway is continuously listening for foreground frames.
# Messages pushed by other nodes (running for example the sensor_push app) will be received by the gateway node,
# transmitted over serial and the received_command_callback() function below will be called.

def received_command_callback(cmd):
  print cmd

argparser = argparse.ArgumentParser()
argparser.add_argument("-d", "--device", help="serial device /dev file modem",
                            default="/dev/ttyUSB0")
argparser.add_argument("-r", "--rate", help="baudrate for serial device", type=int, default=115200)
argparser.add_argument("-v", "--verbose", help="verbose", default=False, action="store_true")
config = argparser.parse_args()

modem = Modem(config.device, config.rate, unsolicited_response_received_callback=received_command_callback, show_logging=config.verbose)

while True:
  pass
