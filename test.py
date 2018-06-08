#!/usr/bin/env python3

import requests
import signal

class Timeout(Exception):
    pass

def notice_timeout(*args):
    raise Timeout

signal.signal(signal.SIGALRM, notice_timeout)
try:
# set alarm to automatically terminate the GET request
    signal.alarm(2)
    requests.get('http://localhost:8080/main.cc')
except Timeout:
    print('Timeout!')
# turn off alarm
signal.alarm(0)
