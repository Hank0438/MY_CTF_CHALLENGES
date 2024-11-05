from base64 import b64decode
from lazy7 import lz77_decompress
from binascii import unhexlify
import struct

# Big Endian
def u8(data):
    return struct.unpack('>B', data)[0]

def u16(data):
    return struct.unpack('>H', data)[0]

def u32(data):
    return struct.unpack('>I', data)[0]

def hex2token(hex_data):
    token_list = []
    token_list_size = len(hex_data)//5
    for i in range(token_list_size):
        distance = hex_data[(i*5):(i*5)+2]
        length = hex_data[(i*5)+2:(i*5)+4]
        char = hex_data[(i*5)+4:(i*5)+5]
        tmp = (u16(distance), u16(length), u8(char))
        token_list.append(tmp)
    return token_list

log_data = b""
with open("output", "rb") as f:
    log_data = f.read()

hex_compressed_data = unhexlify(log_data[13:].strip())

token_list = hex2token(hex_compressed_data)

hex_decompressed_data = lz77_decompress(token_list)
hex_decompressed_data = unhexlify(hex_decompressed_data)

token_list2 = hex2token(hex_decompressed_data)

decompressed_data = lz77_decompress(token_list2)

data = b64decode(decompressed_data)
with open("recovered_flag.png", "wb") as fw:
    fw.write(data)
