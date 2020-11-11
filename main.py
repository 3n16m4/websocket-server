#!/usr/bin/python3
import socket
from json import dumps

request = {
    "id": 3,
    "email": "devatandromeda@gmail.com",
    "password": "katsura",
    "locale": "de_DE",
    "token": "18d27044-2a03-478f-90c6-7e2d76534057",
    "platformGameAccountId": "b33be608-9267-44b6-8186-cbb5bf53f852"
}

def main():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        req = dumps(request) + '\r\n\r\n'
        s.connect(('127.0.0.1', 8081))
        s.sendall("HELLO".encode())
        data = s.recv(1024 * 8)
        print("+++ Received +++ :: {}".format(data.decode('utf-8')))

if __name__ == '__main__':
    main()
