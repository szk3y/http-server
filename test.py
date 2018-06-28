#!/usr/bin/env python3

import requests
from subprocess import Popen
import sys

target = './http-server'

def main(_timeout=None):
    req = requests.get('http://localhost:8080', timeout=_timeout)
    print(req.status_code)
    print(req.reason)
    print(req.headers)
    req = requests.get('http://localhost:8080/hello.html', timeout=_timeout)
    print(req.status_code)
    print(req.reason)
    print(req.headers)

if __name__ == '__main__':
    if 2 <= len(sys.argv) and sys.argv[1] == '--gdb':
        main()
    else:
        p = Popen([target])
        try:
            main(1)
            p.terminate()
        except Exception:
            p.terminate()
            print('Exception!')
