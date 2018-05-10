import socket
import sys

if len(sys.argv) != 4:
    print("Arguments are bad.")
    exit(1)

ip = sys.argv[1]
server_port = sys.argv[2]
client_port = sys.argv[3]

server_facing_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_facing_socket.connect((ip, int(server_port)))

listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
listener.bind(('127.0.0.1', int(client_port)))
listener.listen(5)


client_facing_socket, address = listener.accept()

while 1:
    req = client_facing_socket.recv(2048)

    if req == '':
        print "Client closed connection."
        break

    print "REQ: ", repr(req)
    server_facing_socket.send(req)
    resp = server_facing_socket.recv(2048)
    print "RESP:", repr(resp)
    client_facing_socket.send(resp)
    