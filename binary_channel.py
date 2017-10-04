import random


def bsc(sequence, prob):

    output = bytearray()
    for c_byte in sequence:
        for pos in range(0, 8):
            w = random.uniform(0,1)
            coin = w < prob
            c_byte = c_byte ^ (coin << pos)
        output.append(c_byte)

    return output


x = [1,2,3,4]
print(x)
mybytes = bytearray(x)
print(mybytes)

z = bsc(mybytes, 0.2)
print(z)