#!/usr/bin/env python3

import requests
from subprocess import Popen
import sys

target = './http-server'

def main():
    try:
        req = requests.head('http://localhost:8080', timeout=1)
        print(req.status_code)
        print(req.reason)
        print(req.headers)
    except requests.exceptions.Timeout:
        print('Timeout!')

if __name__ == '__main__':
    if 2 <= len(sys.argv) and sys.argv[1] == '--gdb':
        main()
    else:
        p = Popen([target])
        main()
        p.terminate()
