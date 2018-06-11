#!/usr/bin/env python3

import requests
from subprocess import Popen

target = './http-server'

p = Popen([target])
try:
    req = requests.head('http://localhost:8080', timeout=1)
    print(req.status_code)
    print(req.reason)
    print(req.headers)
except requests.exceptions.Timeout:
    print('Timeout!')
p.terminate()
