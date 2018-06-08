#!/usr/bin/env python3

import requests
import signal
from subprocess import Popen

class Timeout(Exception):
    pass

def notice_timeout(*args):
    raise Timeout

target = './http-server'

signal.signal(signal.SIGALRM, notice_timeout)
p = Popen([target])
try:
# set alarm to automatically terminate the GET request
    signal.alarm(1)
    requests.get('http://localhost:8080/main.cc')
except Timeout:
    print('Timeout!')
# turn off alarm
signal.alarm(0)
p.terminate()
