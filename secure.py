#!/usr/bin/python3
import socket
import ssl
from sys import argv

HOSTNAME = 'localhost'
PORT = 81

def main():
    # PROTOCOL_TLS_CLIENT requires valid cert chain and hostname
    # context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
    context = ssl.create_default_context(ssl.Purpose.SERVER_AUTH)
    context.verify_mode = ssl.CERT_REQUIRED
    context.load_verify_locations('/home/memcpy/src/websocket-server/cabundle.pem')
    context.check_hostname = False

    print('Protocol version: {}'.format(context.protocol))

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        with context.wrap_socket(s, server_hostname = HOSTNAME) as ssock:
            ssock.connect((HOSTNAME, PORT))

            ssock.sendall("Hello World".encode())

            data = ssock.recv(1024 * 8)
            print("+++ Received +++ :: {}".format(data.decode('utf-8')))

            ssock.close()

if __name__ == '__main__':
    main()
