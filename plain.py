#!/usr/bin/python3
import socket
from sys import argv

HOSTNAME = 'localhost'
PORT = 90

def main():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOSTNAME, PORT))

        s.sendall("Hello World".encode())

        data = s.recv(1024 * 8)
        print("+++ Received +++ :: {}".format(data.decode('utf-8')))

        s.close()

if __name__ == '__main__':
    main()

