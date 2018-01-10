
from __future__ import print_function
import argparse
import pprint

import jsonpickle

from d7a.alp.command import Command
from d7a.system_files.dll_config import DllConfigFile

from d7a.alp.interface import InterfaceType
from d7a.d7anp.addressee import Addressee, IdType
from d7a.sp.configuration import Configuration as D7config
from d7a.sp.qos import QoS, ResponseMode
from d7a.system_files.uid import UidFile

from tb_api_client import swagger_client
from tb_api_client.swagger_client import ApiClient, Configuration
from tb_api_client.swagger_client.rest import ApiException

from time import sleep
import sys

def start_api(config):
    global device_controller_api
    global device_api_controller_api
    global api_client

    api_client_config = Configuration()
    api_client_config.host = config.url
    api_client_config.api_key['X-Authorization'] = config.token
    api_client_config.api_key_prefix['X-Authorization'] = 'Bearer'
    api_client = ApiClient(api_client_config)

    device_controller_api = swagger_client.DeviceControllerApi(api_client=api_client)
    device_api_controller_api = swagger_client.DeviceApiControllerApi(api_client=api_client)

def send_json(config, json_data):

        try:
            # first get the deviceId mapped to the device name
            response = device_controller_api.get_tenant_device_using_get(device_name=str(config.node))
            device_id = response.id.id
            # print(device_id)

            # next, get the access token of the device
            response = device_controller_api.get_device_credentials_by_device_id_using_get(device_id=device_id)
            device_access_token = response.credentials_id
            # print(device_access_token)

            # finally, store the sensor attribute on the node in TB
            response = device_api_controller_api.post_telemetry_using_post(
                device_token=device_access_token,
                json = json_data
            )

            print("Updated json telemetry for node {}".format(config.node))
        except ApiException as e:
            print("Exception when calling API: %s\n" % e)

def execute_rpc_command(config, rpc_data):
        cmd = Command.create_with_return_file_data_action(file_id=40,
                                                          data=rpc_data, interface_type=InterfaceType.D7ASP,
                                                          interface_configuration=D7config(
                                                              qos=QoS(resp_mod=ResponseMode.RESP_MODE_NO),
                                                              addressee=Addressee(access_class=0x11,
                                                                                  id_type=IdType.NOID)))

        # we will do it by a manual POST to /api/plugins/rpc/oneway/ , which is the route specified
        # in the documentation
        cmd = {"method": "execute-alp-async", "params": jsonpickle.encode(rpc_data), "timeout": 500}
        path_params = {'deviceId': config.device}
        query_params = {}
        header_params = {}
        header_params['Accept'] = api_client.select_header_accept(['*/*'])
        header_params['Content-Type'] = api_client.select_header_content_type(['application/json'])

        # Authentication setting
        auth_settings = ['X-Authorization']
        return api_client.call_api('/api/plugins/rpc/oneway/{deviceId}', 'POST',
                                        path_params,
                                        query_params,
                                        header_params,
                                        body=cmd,
                                        post_params=[],
                                        files={},
                                        response_type='DeferredResultResponseEntity',
                                        auth_settings=auth_settings,
                                        async=False)
