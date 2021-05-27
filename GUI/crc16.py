def generate_table():
    result = []
    polynomial = 0x1021
    symbol = 0
    while True:
        remainder = symbol
        for i in range(8):
            if remainder & 0b10000000:
                remainder = (remainder << 1) ^ polynomial
            else:
                remainder = remainder << 1

            remainder &= 0xFFFF
        result.append(remainder)
        symbol += 1
        if symbol >> 8:
            break
    return result


def generate_crc(string, table):
    crc = 0x0000
    for i in range(len(string)):
        crc = crc << 8 ^ table[ord(string[i]) ^ (crc >> 8)]
        crc &= 0xFFFF
    return crc


if __name__ == '__main__':
    crc_table = generate_table()
    print(crc_table)
    word = 'Pavl'
    total_crc = generate_crc(word, crc_table)
    print(total_crc)
    word += chr(total_crc >> 8) + chr(total_crc & 0xFF)
    print(word)
    print(generate_crc(word, crc_table))
