from tb_api_client import swagger_client
from tb_api_client.swagger_client import ApiClient, Configuration
from tb_api_client.swagger_client.rest import ApiException

def start_api(config):
    global device_controller_api
    global device_api_controller_api

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
