#!/usr/bin/env python
import argparse
import json

import eventlet
from datetime import time, datetime
import jsonpickle
from flask import Flask, render_template, request, jsonify
from flask_socketio import SocketIO, emit
from flask_cors import CORS, cross_origin

from d7a.alp.command import Command
from d7a.alp.interface import InterfaceType
from d7a.d7anp.addressee import IdType, Addressee
from d7a.phy.channel_header import ChannelBand, ChannelCoding, ChannelClass
from d7a.sp.configuration import Configuration
from d7a.sp.qos import QoS, ResponseMode
from d7a.system_files.system_files import SystemFiles
from d7a.system_files.system_file_ids import SystemFileIds
from d7a.types.ct import CT
from modem.modem import Modem

app = Flask(__name__, static_url_path='/static')
app.config['SEND_FILE_MAX_AGE_DEFAULT'] = 0 # do not cache static assets for now
CORS(app) # TODO can be removed after integrating the ng project (and serving ng through flask)

socketio = SocketIO(app)
eventlet.monkey_patch()
modem = None


@app.route('/')
def index():
  return render_template('index.html',
                         systemfiles=SystemFiles().get_all_system_files(),
                         qos_response_modes=ResponseMode.__members__,
                         id_types=IdType.__members__)

@app.route('/systemfiles')
def get_system_files():
  options = []
  for file in SystemFiles().get_all_system_files():
    options.append({"file_id": file.value, "file_name": file.name, "data": ""})

  return jsonify(options)

@app.route('/idtypes')
def get_id_types():
  id_types = []
  for name, member in IdType.__members__.items():
    id_types.append({'id': name, 'value': name})

  return jsonify(id_types)

@app.route('/responsemodes')
def get_response_modes():
  response_modes = []
  for name, member in ResponseMode.__members__.items():
    response_modes.append({"id": name, "value": name})

  return jsonify(response_modes)

@app.route('/channel_bands')
def get_channel_bands():
  channel_bands = []
  for name, member in ChannelBand.__members__.items():
    channel_bands.append({"id": name, "value": name})

  return jsonify(channel_bands)

@app.route('/channel_codings')
def get_channel_codings():
  channel_codings = []
  for name, member in ChannelCoding.__members__.items():
    channel_codings.append({"id": name, "value": name})

  return jsonify(channel_codings)

@app.route('/channel_classes')
def get_channel_classes():
  channel_classes = []
  for name, member in ChannelClass.__members__.items():
    channel_classes.append({"id": name, "value": name})

  return jsonify(channel_classes)


@socketio.on('execute_raw_alp')
def on_execute_raw_alp(data):
  alp_hex_string = data['raw_alp'].replace(" ", "").strip()
  modem.execute_command_async(bytearray(alp_hex_string.decode("hex")))

@socketio.on('read_local_system_file')
def on_read_local_system_file(data):
  cmd = Command.create_with_read_file_action_system_file(SystemFiles.files[SystemFileIds(int(data['system_file_id']))])
  print("executing cmd: {}".format(cmd))
  modem.execute_command_async(cmd)

  return {'tag_id': cmd.tag_id}

@socketio.on('write_local_system_file')
def on_write_local_system_file(data):
  file = jsonpickle.decode(json.dumps(data))
  cmd = Command.create_with_write_file_action_system_file(file)
  print("executing cmd: {}".format(cmd))
  modem.execute_command_async(cmd)
  return {'tag_id': cmd.tag_id}

@socketio.on('read_local_file')
def on_read_local_file(data):
  print("read_local_file")
  cmd = Command.create_with_read_file_action(
    file_id=int(data['file_id']),
    offset=int(data['offset']),
    length=int(data['length'])
  )

  modem.execute_command(cmd)

@socketio.on('execute_command')
def execute_command(data):
  print("execute_command")
  print data

  interface_configuration = None
  interface_type = InterfaceType(int(data["interface"]))
  if interface_type == InterfaceType.D7ASP:
    id_type = IdType[data["id_type"]]
    id = int(data["id"])
    if id_type == IdType.NOID:
      id = None
    if id_type == IdType.NBID:
      id = CT() # TODO convert

    interface_configuration = Configuration(
      qos=QoS(resp_mod=ResponseMode[data["qos_response_mode"]]),
      addressee=Addressee(
        access_class=int(data["access_class"]),
        id_type=id_type,
        id=id
      )
    )

  cmd = Command.create_with_read_file_action(
    interface_type=interface_type,
    interface_configuration=interface_configuration,
    file_id=int(data["file_id"]),
    offset=int(data["offset"]),
    length=int(data["length"])
  )

  print("executing cmd: {}".format(cmd))
  modem.execute_command_async(cmd)
  return {
    'tag_id': cmd.tag_id,
    'interface': interface_type.name,
    'command_description': cmd.describe_actions()
  }


@socketio.on('connect')
def on_connect():
  global modem
  if modem == None:
    modem = Modem(config.device, config.rate, command_received_callback)

  print("modem: " + str(modem.uid))
  emit('module_info', {
    'uid': modem.uid,
    'application_name': modem.firmware_version.application_name,
    'git_sha1': modem.firmware_version.git_sha1,
    'd7ap_version': modem.firmware_version.d7ap_version
  }, broadcast=True)


@socketio.on('disconnect')
def on_disconnect():
  print('Client disconnected', request.sid)


def command_received_callback(cmd):
  print("cmd received: {}".format(cmd))
  with app.test_request_context('/'):
    socketio.emit("received_alp_command", {
      'tag_id': cmd.tag_id,
      'recv_ts': datetime.now().isoformat(),
      'response_command_description': cmd.describe_actions(),
      'response_command': json.loads(jsonpickle.encode(cmd)) # we use jsonpickle here as an easy way to serialize the whole object structure
    }, broadcast=True)

    print("broadcasted recv command")

@socketio.on_error_default
def default_error_handler(e):
  print("Error {} in {} with args".format(e, request.event["message"], request.event["args"]))

if __name__ == '__main__':
  argparser = argparse.ArgumentParser()
  argparser.add_argument("-d", "--device", help="serial device /dev file modem",
                         default="/dev/ttyUSB0")
  argparser.add_argument("-r", "--rate", help="baudrate for serial device", type=int, default=115200)
  argparser.add_argument("-p", "--port", help="TCP port used by webserver", type=int, default=5000)
  config = argparser.parse_args()
  modem = None
  socketio.run(app, debug=True, host="0.0.0.0", port=config.port)
