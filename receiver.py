import socket
import zlib
import reedsolo as solomon


def get_packet_label(packet):
    return packet[-1]

TCP_IP = '127.0.0.1'
TCP_PORT = 5005

REDUNDANCY = 30
BUFFER_SIZE = 40 + 1
NACK = bytearray([0b10110010, 0b11010011])
ACK = bytearray([0b11001000, 0b00111010])

rs = solomon.RSCodec(REDUNDANCY)

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((TCP_IP, TCP_PORT))
s.listen(1)

conn, addr = s.accept()
received_message = bytearray()

print('Connection address:', addr)
i = 0  # Segur que hi ha alguna manera de fer-ho sense indexes

r_next = 0
while True:
    data = conn.recv(BUFFER_SIZE + REDUNDANCY)
    b_data = bytearray(data)
    if not data: break
    print('Length new data:', len(data))

    try:
        c_data = rs.decode(b_data)
        s_last = get_packet_label(c_data)

        print("Good transmission")
        if s_last == r_next:
            r_next ^= 1
            for k in c_data:
                received_message.append(k)

            aux_ack = list(ACK)
            aux_ack.append(r_next)
            en_ack = rs.encode(aux_ack)
            conn.send(en_ack)

    except solomon.ReedSolomonError:
        print("Error-waiting for retransmision")
        aux_nack = list(NACK)
        aux_nack.append(r_next)
        en_nack = rs.encode(aux_nack)
        conn.send(en_nack)

    #print('Accumulated received message:', received_message)



decompressed_m = zlib.decompress(received_message)
print('Received message:', decompressed_m)
conn.close()