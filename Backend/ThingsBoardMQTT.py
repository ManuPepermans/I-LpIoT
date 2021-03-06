import paho.mqtt.client as mqtt
import signal
import time
import argparse
import json

def start_mqtt(broker, config):
    global mqtt_config
    mqtt_config = mqtt.Client("", True, None, mqtt.MQTTv31)
    mqtt_config.username_pw_set(config.tokenmqtt)
    mqtt_config.connect(broker, 1883, 60)
    mqtt_config.topic = config.telemetry

def on_mqtt_publish(lan, lon):
    try:
        current = 	int(round(time.time() * 1000))
        data_str = {"GROUP1": [{"ts": current,"values": {"lat": lan,"lon": lon}}]}

        mqtt_config.publish(mqtt_config.topic, str(data_str), 1)
        print("Publish: {}".format(str(data_str)))

    except:
        print("Error")