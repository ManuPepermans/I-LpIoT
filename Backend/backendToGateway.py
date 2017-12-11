#!/usr/bin/env python

from __future__ import print_function
import argparse
import pprint

import jsonpickle

from d7a.alp.command import Command
from d7a.system_files.dll_config import DllConfigFile
from tb_api_client import swagger_client
from tb_api_client.swagger_client import ApiClient, Configuration



class backendToGateway:
  def __init__(self):
    argparser = argparse.ArgumentParser()
    argparser.add_argument("-v", "--verbose", help="verbose", default=False, action="store_true")
    argparser.add_argument("-u", "--url", help="URL of the thingsboard server", default="http://localhost:8080")
    argparser.add_argument("-t", "--token", help="token to access the thingsboard API", required=True)
    argparser.add_argument("-d", "--device", help="device ID of the gateway modem to send the command to", required=True)

    self.config = argparser.parse_args()

    api_client_config = Configuration()
    api_client_config.host = self.config.url
    api_client_config.api_key['X-Authorization'] = self.config.token
    api_client_config.api_key_prefix['X-Authorization'] = 'Bearer'
    self.api_client = ApiClient(api_client_config)

  def execute_rpc_command(self, device_id, json_alp_cmd):
    # we will do it by a manual POST to /api/plugins/rpc/oneway/ , which is the route specified
    # in the documentation
    cmd = {"method": "execute-alp-async", "params":  jsonpickle.encode(json_alp_cmd), "timeout": 500 }
    path_params = { 'deviceId': device_id }
    query_params = {}
    header_params = {}
    header_params['Accept'] = self.api_client.select_header_accept(['*/*'])
    header_params['Content-Type'] = self.api_client.select_header_content_type(['application/json'])

    # Authentication setting
    auth_settings = ['X-Authorization']
    return self.api_client.call_api('/api/plugins/rpc/oneway/{deviceId}', 'POST',
                                    path_params,
                                    query_params,
                                    header_params,
                                    body=cmd,
                                    post_params=[],
                                    files={},
                                    response_type='DeferredResultResponseEntity',
                                    auth_settings=auth_settings,
                                    async=False)

  def run(self):
    cmd = {0x32, 0xd7, 0x01, 0x00, 0x10, 0x01, 0x20, 0x01, 0x00}
    self.execute_rpc_command(self.config.device, cmd)


if __name__ == "__main__":
  backendToGateway().run()
