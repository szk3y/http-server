#!/usr/bin/env python3

import requests
from subprocess import Popen

target = './http-server'

p = Popen([target])
try:
    requests.get('http://localhost:8080/main.cc', timeout=1)
except requests.exceptions.Timeout:
    print('Timeout!')
p.terminate()
