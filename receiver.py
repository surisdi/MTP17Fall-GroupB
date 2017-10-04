import socket
import zlib
import reedsolo as solomon

TCP_IP = '127.0.0.1'
TCP_PORT = 5005

REDUNDANCY = 10
BUFFER_SIZE = 40

rs = solomon.RSCodec(REDUNDANCY)

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((TCP_IP, TCP_PORT))
s.listen(1)

conn, addr = s.accept()
received_message = bytearray()

print('Connection address:', addr)
i = 0  # Segur que hi ha alguna manera de fer-ho sense indexes
while 1:
    data = conn.recv(BUFFER_SIZE + REDUNDANCY)
    b_data = bytearray(data)
    print(b_data)
    if not data: break
    print('Length new data:', len(data))

    c_data = rs.decode(data)
    for k in c_data:
        received_message.append(k)

    #print('Accumulated received message:', received_message)

decompressed_m = zlib.decompress(received_message)
print('Received message:', decompressed_m)
conn.close()