import socket
import zlib

TCP_IP = '127.0.0.1'
TCP_PORT = 5005
BUFFER_SIZE = 1024  # No necessariament el mateix que al client

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((TCP_IP, TCP_PORT))
s.listen(1)

conn, addr = s.accept()
received_message = ''
print 'Connection address:', addr
i = 0  # Segur que hi ha alguna manera de fer-ho sense indexes
while 1:
    data = conn.recv(BUFFER_SIZE)
    if not data: break
    print 'Length new data:', len(data)
    received_message += data
    print 'Accumulated received message:', received_message

decompressed_m = zlib.decompress(received_message)
print 'Received message:', decompressed_m
conn.close()