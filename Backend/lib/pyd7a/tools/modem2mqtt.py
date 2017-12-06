#!/usr/bin/env python

import argparse
import serial
import time

import paho.mqtt.client as mqtt

from modem.modem import Modem


class Modem2Mqtt():

  def __init__(self):
    argparser = argparse.ArgumentParser()
    argparser.add_argument("-d", "--device", help="serial device /dev file modem",
                           default="/dev/ttyUSB0")
    argparser.add_argument("-r", "--rate", help="baudrate for serial device", type=int, default=115200)
    argparser.add_argument("-v", "--verbose", help="verbose", default=False, action="store_true")
    argparser.add_argument("-b", "--broker", help="mqtt broker hostname",
                             default="localhost")

    self.serial = None
    self.modem_uid = None
    self.bridge_count = 0
    self.next_report = 0

    self.config = argparser.parse_args()
    self.setup_modem()
    self.connect_to_mqtt()

  def setup_modem(self):
    # we use Modem here only for reading the modem information, not for parsing.
    # reading the bytes from serial (after initial connect) is not done using Modem but overridden here
    modem = Modem(self.config.device, self.config.rate, None, show_logging=self.config.verbose)
    self.serial = modem.dev
    self.modem_uid = modem.uid
    modem.stop_reading()  # so we can read the serial stream ourself

  def connect_to_mqtt(self):
    self.connected_to_mqtt = False

    self.mq = mqtt.Client("", True, None, mqtt.MQTTv31)
    self.mq.on_connect = self.on_mqtt_connect
    self.mq.on_message = self.on_mqtt_message
    self.mqtt_topic_incoming = "/DASH7/{}/incoming".format(self.modem_uid)
    self.mqtt_topic_outgoing = "/DASH7/{}/outgoing".format(self.modem_uid)
    self.mq.connect(self.config.broker, 1883, 60)
    self.mq.loop_start()
    while not self.connected_to_mqtt: pass  # busy wait until connected
    print("Connected to MQTT broker on {}, sending to topic {} and subscribed to topic {}".format(
      self.config.broker,
      self.mqtt_topic_incoming,
      self.mqtt_topic_outgoing
    ))

  def on_mqtt_connect(self, client, config, flags, rc):
    self.mq.subscribe(self.mqtt_topic_outgoing)
    self.connected_to_mqtt = True

  def on_mqtt_message(self, client, config, msg):
    print("on_message") # TODO
    # try:    self.handle_msg(msg.topic, msg.payload)
    # except:
    #   exc_type, exc_value, exc_traceback = sys.exc_info()
    #   lines = traceback.format_exception(exc_type, exc_value, exc_traceback)
    #   trace = "".join(lines)
    #   self.log("failed to handle incoming message:", msg.payload, trace)

  def publish_to_mqtt(self, msg):
    self.mq.publish(self.mqtt_topic_incoming, msg)

  def __del__(self): # pragma: no cover
    try:
      self.mq.loop_stop()
      self.mq.disconnect()
    except: pass


  def run(self):
    print("Started")
    keep_running = True
    while keep_running:
      try:
        data = self.serial.read()
        if data:
          data = bytearray(data)
          self.publish_to_mqtt(data)
          self.keep_stats()
      except serial.SerialException:
        time.sleep(1)
        print("resetting serial connection...")
        self.setup_modem()
        return
      except KeyboardInterrupt:
        print("received KeyboardInterrupt... stopping processing")
        keep_running = False

      self.report_stats()

  def keep_stats(self):
    self.bridge_count += 1

  def report_stats(self):
    if self.next_report < time.time():
      if self.bridge_count > 0:
        print("bridged %s messages" % str(self.bridge_count))
        self.bridge_count = 0
      self.next_report = time.time() + 15 # report at most every 15 seconds

if __name__ == "__main__":
  Modem2Mqtt().run()
