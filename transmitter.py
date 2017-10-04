import socket
import zlib
import reedsolo as solomon
import random

TCP_IP = '127.0.0.1'
TCP_PORT = 5005

REDUNDANCY = 30
BUFFER_SIZE = 40

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

for i in range(0, len(compressed_f) // BUFFER_SIZE + 1):
    if (i + 1) * BUFFER_SIZE > len(compressed_f):  # last packet may be smaller
        packet = compressed_f[i * BUFFER_SIZE:len(compressed_f)]
        rs_pack = rs.encode(packet)
        noisy_pack = bsc(rs_pack, 0.01)
    else:  # Normal case
        packet = compressed_f[i * BUFFER_SIZE:(i + 1) * BUFFER_SIZE]
        rs_pack = rs.encode(packet)
        noisy_pack = bsc(rs_pack, 0.01)
        print("Packet ", packet)
        print("\n\n")
        print("PacketRS ", rs_pack)
        print("\n\n")
        print("Packet Noisy", noisy_pack)
        print("\n\n")
    s.send(noisy_pack)

s.close()