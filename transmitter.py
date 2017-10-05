import socket
import zlib
import reedsolo as solomon
import random

TCP_IP = '127.0.0.1'
TCP_PORT = 5005

REDUNDANCY = 30
BUFFER_SIZE = 40

NACK = bytearray([0b10110010, 0b11010011])
ACK = bytearray([0b11001000, 0b00111010])


# Symmetric binary channel. prob: float in [0, 1] Transition/Error probability
def bsc(sequence, prob):

    output = bytearray()
    for c_byte in sequence:
        for pos in range(0, 8):
            w = random.uniform(0,1)
            coin = w < prob
            c_byte = c_byte ^ (coin << pos)
        output.append(c_byte)

    return output


def get_packet_label(seq):
    return seq[-1]




rs = solomon.RSCodec(REDUNDANCY)

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))

# Read file
f = open('text.txt', 'rU').read()
b_f = bytearray(f, "latin1")

# Compress file
compressed_f = zlib.compress(b_f)

print('Original size:', len(f), 'Compressed size:', len(compressed_f))
#print('Bits to be sent:')
#print(' '.join(map(bin, compressed_f[0:BUFFER_SIZE])))

s_last = 0
i = 0
while i < len(compressed_f) // BUFFER_SIZE + 1:
    if (i + 1) * BUFFER_SIZE > len(compressed_f):  # last packet may be smaller
        packet = compressed_f[i * BUFFER_SIZE:len(compressed_f)]
        packet += bytes([s_last])

        rs_pack = rs.encode(packet)
        noisy_pack = bsc(rs_pack, 0.05)
    else:  # Normal case
        packet = compressed_f[i * BUFFER_SIZE:(i + 1) * BUFFER_SIZE]
        packet += bytes([s_last])

        rs_pack = rs.encode(packet)
        noisy_pack = bsc(rs_pack, 0.05)
        print("\n\n Packet ", packet, "\n\n", "PacketRS ", rs_pack,
              "\n\n", "Packet Noisy", noisy_pack)
    s.send(noisy_pack)

    rcv_ack = s.recv(BUFFER_SIZE+1)
    rcv_ack = rs.decode(rcv_ack)

    r_next = get_packet_label(rcv_ack)

    if rcv_ack[0:-2] == ACK:
        if r_next != s_last:
            s_last ^= 1
            i += 1


s.close()