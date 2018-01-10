import requests
import json
import jsonpickle

url = "http://thingsboard.idlab.uantwerpen.be:8080/api/auth/login"
data = '{"username":"student@lopow-coaw-lab.uantwerpen.be", "password":"student"}'
headers = {'Content-type': 'application/json', 'Accept': 'application/json'}
r = requests.post(url, data=data, headers=headers)
jwt_token = r.json()['token']
print("Bearer " + jwt_token)