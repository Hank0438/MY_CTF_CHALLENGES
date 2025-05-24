
OFF = 0xdeadbeef
JMP_OFF = [0xeb, 6]
DEC_AL = [0xfe]
SUB_AL_OFF = [0x2c, 5]
ADD_AL_OFF = [0x04, 3]
XOR_AL_OFF = [0x34, 6]
SUB_AL_CL = [0x2a]
ADD_AL_CL = [0x02]
XOR_AL_CL = [0x32]
ROR_AL_2 = [0xc0, 0xc0]
ROL_AL_2 = [0xc0, 0xf9]
ROR_AL_6 = [0xd2, 0xc8]
ROL_AL_6 = [0xd2, 0xf8]

NOP1 = [0x90]
NOP2 = [0xf8]
NOP3 = [0xf9]

inst = []
inst += SUB_AL_OFF
inst += ROL_AL_6
inst += ADD_AL_OFF
inst += XOR_AL_OFF
inst += DEC_AL
inst += JMP_OFF
inst += ROR_AL_2
inst += NOP3
inst += ADD_AL_CL
inst += NOP1
inst += ROL_AL_2
inst += SUB_AL_CL
inst += ROR_AL_6
inst += NOP2
inst += XOR_AL_CL
inst += NOP1
inst += NOP1
inst += NOP3
inst += NOP1
inst += NOP2
inst += NOP2
inst += NOP3
inst += NOP1
inst += NOP2

print("len(inst):", hex(len(inst)))

output = ", ".join([hex(i) for i in inst])

# Read Executable
code = b""
with open("samples/small_print_flag.exe", "rb") as f:
    code = f.read()

code_len = len(code)
print("len(code):", (code_len))


def CLI_ROL(a,b):
    return ((a << b) & 0xff) | (a >> (8-b))
def CLI_ROR(a,b):
    return (a >> b) | ((a << (8-b)) & 0xff)

decryptor_offset = [
    0x2c, 0x5, 0xd2, 0xf8, 0x4, 0x3, 0x34, 0x6, 
    0xfe, 0xeb, 0x6, 0xc0, 0xc0, 0xf9, 0x2, 0x90, 
    0xc0, 0xf9, 0x2a, 0xd2, 0xc8, 0xf8, 0x32, 0x90, 
    0x90, 0xf9, 0x90, 0xf8, 0xf8, 0xf9, 0x90, 0xf8,
    0x0    
]

# VM: generate key stream
al = 0
bl = 0
cl = code_len & 0x7

mapping = [[0]*0x100, [0]*0x100]

for idx, bl in enumerate([0x7f, 0x80]):
    for i in range(0x100):
        print(f"[!] i={hex(i)}")
        al = i
        j = 0
        while (j < 0x20):
            # JMP short
            if (decryptor_offset[j] == 0xeb):
                j += 1
                if (bl < 0x80):
                    j += 1
                    continue
                j += decryptor_offset[j]
                j += 1
            # DEC  AL
            elif (decryptor_offset[j] == 0xfe): 
                al -= 1
                if al < 0:
                    al += 0x100
                j += 1
            # SUB AL,CL
            elif ((decryptor_offset[j] == 0x2a)):
                al -= cl
                if al < 0:
                    al += 0x100
                j += 1
            # ADD AL,CL
            elif ((decryptor_offset[j] == 0x02)):
                al += cl
                al &= 0xff
                j += 1
            # XOR AL,CL
            elif ((decryptor_offset[j] == 0x32)):
                al ^= cl
                j += 1
            # SUB AL,num
            elif ((decryptor_offset[j] == 0x2c)):
                j += 1
                al -= decryptor_offset[j]
                if al < 0:
                    al += 0x100
                j += 1
            # ADD AL,num
            elif ((decryptor_offset[j] == 0x04)):
                j += 1
                al += decryptor_offset[j]
                al &= 0xff
                j += 1
            # XOR AL,num
            elif ((decryptor_offset[j] == 0x34)):
                j += 1
                al ^= decryptor_offset[j]
                j += 1
            # ROL/ROR AL,2
            elif ((decryptor_offset[j] == 0xc0)):
                j += 1
                if (decryptor_offset[j] == 0xc0):
                    j += 1
                    al = CLI_ROL(al, 2)
                else:
                    j += 1
                    al = CLI_ROR(al, 2)
            # ROL/ROR AL,6
            elif ((decryptor_offset[j] == 0xd2)):
                j += 1
                if (decryptor_offset[j] == 0xc8):
                    j += 1
                    al = CLI_ROR(al, 6)
                else:
                    j += 1
                    al = CLI_ROL(al, 6)
            elif ((decryptor_offset[j] == 0x90)):
                j += 1
                continue
            elif ((decryptor_offset[j] == 0xf8)):
                j += 1
                continue
            elif ((decryptor_offset[j] == 0xf9)):
                j += 1
                continue
            else:
                print("Error: Unhandled opcode: ", hex(decryptor_offset[j]))
                exit(-1)
        al &= 0xff
        mapping[idx][i] = al

# Encrypt code
for i in range(code_len):
    idx = 0
    al = i & 0xff
    bl = code[i]
    if (bl >= 0xf0):
        idx = 1

    enc_code = bytes([ bl ^ mapping[idx][i&0xff] ])
    code = code[:i] + enc_code + code[(i+1):]

# Write the hexdump of the executable
with open("enc_print_flag.hex", "w") as f:
    for i in range((len(code)//8)+1):
        f.write(",".join([hex(c) for c in code[i*8:(i+1)*8]]))
        f.write(",\n")