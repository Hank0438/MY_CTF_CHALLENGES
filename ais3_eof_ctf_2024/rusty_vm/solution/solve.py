#!/usr/bin/python
from Crypto.Cipher import ARC4
from Crypto.Hash import SHA256
from Crypto import Random
from binascii import unhexlify
import sys

key = b'SecretKey'
flag = b"AIS3{15_Th3_RuS7_vM_re@LlY_3@5Y_i5NT_I7}" # uid:0, default
vm_output_path = "uid_0_rel.txt"

def enc(key,p):
    return ARC4.new(key).encrypt(p)

def dec(key,msg):
    return ARC4.new(key).decrypt(msg)

def test_rc4():
    key = b'SecretKey'
    p = b'Hello, RC4!'
    #nonce = Random.new().read(16)
    #key += nonce
    #key = SHA256.new(key).digest() #key is no more than 256bytes

    enc_p = enc(key, p)
    _p = dec(key, enc_p)
    print("enc_p: ", [ep for ep in enc_p])
    print("_p: ", _p)
    # print(dec(key,enc(key,p)))

def shuffle_flag(flag:list):
    mapping = [2, 36, 9, 39, 16, 35, 14, 12, 11, 37, 1, 26, 29, 27, 32, 31, 34, 23, 30, 0, 4, 19, 15, 6, 33, 17, 24, 22, 28, 38, 25, 20, 5, 10, 7, 18, 13, 21, 3, 8];
    return [flag[i] for i in mapping]

def rev_shuffle_flag(shuffled_flag:list):
    mapping = [2, 36, 9, 39, 16, 35, 14, 12, 11, 37, 1, 26, 29, 27, 32, 31, 34, 23, 30, 0, 4, 19, 15, 6, 33, 17, 24, 22, 28, 38, 25, 20, 5, 10, 7, 18, 13, 21, 3, 8];
    flag = [0]*40
    for idx, m in enumerate(mapping):
        flag[m] = shuffled_flag[idx]
    return flag

def get_output():
    output_o = []
    output_s = []
    last_vm_line_idx = 0
    with open(vm_output_path, "r") as f:
        lines = f.readlines()
        for idx, line in enumerate(lines):
            if line.startswith("[VM]"):
                if idx == (len(lines)-1):
                    break    
                last_vm_line_idx = idx

        last_vm_line = lines[last_vm_line_idx]
        if "instruction: 2\n" in last_vm_line:
            last_vm_line_idx += 1

        for idx, line in enumerate(lines):

            if line.startswith("[OUTPUT]"):
                num = int(line.strip().split()[1])
                if idx <= last_vm_line_idx:
                    output_o.append(num)
                else:
                    output_s.append(num)


    # print(output)
    return output_o, output_s

def get_instructions():
    instructions = []
    with open("vm_instructions.txt", "r") as f:
        lines = f.readlines()
        for line in lines:
            if "//" in line:
                inst = [int(i.strip(), 0) for i in line.strip().split(",")[:-1] ]
                instructions.append(inst)
    # print(instructions)
    return instructions

    
'''
PUSH = 0x01 #=> self.instruction_push(),
POP = 0x02 #=> self.instruction_pop(),
ADD = 0x03 #=> self.instruction_add(),
SUB = 0x04 #=> self.instruction_sub(),
XOR = 0x0A #=> self.instruction_xor(),
NOT = 0x0B #=> self.instruction_not(),
ROL = 0x0C #=> self.instruction_rol(),
ROR = 0x0D #=> self.instruction_ror(),
DUP = 0x0E #=> self.instruction_dup(),
SWAP = 0x0F #=> self.instruction_swap(),
ROT = 0x10 #=> self.instruction_rot(),
OVER = 0x11 #=> self.instruction_over(),
HALT = 0x12 #=> self.instruction_halt(),
PUTS = 0x13 #=> self.instruction_puts_char(),
JE = 0x14 #=> self.instruction_branch_if_zero(),
JNE = 0x15 #=> self.instruction_branch_if_positive(),
NOP = 0x17 #=> self.instruction_nop(),
'''

# reconstruct stack
def rev_vm(output_o, output_s, instructions):

    loc = 0
    output = output_o
    stack = output_s[::-1] 
    idx_je = list(filter(lambda x: x[1][0]==0x14, zip(range(len(instructions)), instructions)  ))
    idx_jne = list(filter(lambda x: x[1][0]==0x15, zip(range(len(instructions)), instructions)  ))
    # idx_pop = list(filter(lambda x: x[1][0]==0x2, zip(range(len(instructions)), instructions)  ))
    # if len(idx_pop) < len(output):
    #     for _ in range(len(idx_pop), len(output)):
    #         stack.append(output.pop())

    print(idx_je)
    print(idx_jne)

    while len(instructions) > 0:

        inst = instructions.pop()
        opcode = inst[0]
        if len(inst) > 1:
            arg = inst[1]
        if len(inst) > 2:
            raise Exception("parsing instructions wrong.")

        print(f"[+] cur: {len(instructions)}, opcode: {opcode}")
        print(f"\tstack: {stack}")
        print(f"\toutput: {output}")



        if opcode == 0x1: #PUSH
            assert(arg == stack.pop())

        elif opcode == 0x2: #POP
            stack.append(output.pop())

        elif opcode == 0x3: #ADD
            inst_p = instructions.pop()
            opcode_p = inst_p[0]
            assert(opcode_p == 0x1) # must be push
            arg_p = inst_p[1]
            a = stack.pop()
            b = (a-arg_p+0x100)&0xff
            stack.append(b)

        elif opcode == 0x4: #SUB
            inst_p = instructions.pop()
            opcode_p = inst_p[0]
            assert(opcode_p == 0x1) # must be push
            arg_p = inst_p[1]
            a = stack.pop()
            if a > arg_p:
                a = ~a + 1
            b = (a+arg_p)&0xff
            stack.append(b)

        elif opcode == 0xa: #XOR
            inst_p = instructions.pop()
            opcode_p = inst_p[0]
            assert(opcode_p == 0x1) # must be push
            arg_p = inst_p[1]
            a = stack.pop()
            b = (a^arg_p)
            stack.append(b)

        elif opcode == 0xb: #NOT
            a = stack.pop()
            b = (a^0xff)
            stack.append(b)

        elif opcode == 0xc: #ROL
            inst_p = instructions.pop()
            opcode_p = inst_p[0]
            assert(opcode_p == 0x1) # must be push
            arg_p = inst_p[1]
            a = stack.pop()
            b = (a >> arg_p | a << (8-arg_p)) & 0xff
            stack.append(b)

        elif opcode == 0xd: #ROR
            inst_p = instructions.pop()
            opcode_p = inst_p[0]
            assert(opcode_p == 0x1) # must be push
            arg_p = inst_p[1]
            a = stack.pop()
            b = (a << arg_p | a >> (8-arg_p)) & 0xff
            stack.append(b)

        elif opcode == 0xe: #DUP
            assert(stack[-2] == stack[-1]) 
            stack.pop()

        elif opcode == 0xf: #SWAP
            a = stack.pop() # b
            b = stack.pop() # a
            stack.append(a)
            stack.append(b)

        elif opcode == 0x10: #ROT
            c = stack.pop()
            a = stack.pop() 
            b = stack.pop() 
            stack.append(c)
            stack.append(b)
            stack.append(a)

        elif opcode == 0x11: #OVER
            a = stack.pop() 
            b = stack.pop()
            c = stack.pop() 
            assert(a == c) 
            stack.append(c)
            stack.append(b)


        elif opcode == 0x17: #NOP
            pass
        elif opcode in [0x14, 0x15]: #JE / JNE
            pass
        else:
            raise Exception("unknown instruction: ", inst)

        # handle JE or JNE
        l = len(instructions)
        for i in range(min(0x10, l)):
            opcode_j = instructions[-i][0]
            # if opcode == 0x14: #JE
            # if opcode == 0x15: #JNE
            if opcode_j in [0x14, 0x15]:
                arg_j = instructions[-i][1]
                if arg_j != i-1:
                    break
                print("found branch instruction: ", opcode_j)
                assert(instructions[-i-1][0] == 0x1) # must be PUSH
                if opcode_j == 0x14:
                    bp = (instructions[-i-1][1] == 0)
                if opcode_j == 0x15:
                    bp = (instructions[-i-1][1] != 0)


                if bp:
                    for _ in range(arg_j):
                        instructions.pop()
                    inst_j = instructions.pop() # JE or JNE
                    inst_p = instructions.pop() # PUSH
                    print(inst_j)
                    print(inst_p)
                else:
                    instructions[-i] = [0x17]
                    instructions[-i-1] = [0x17]


                break

    return stack


def main():
    global key
    global flag
    global vm_output_path
    
    vm_output_path = sys.argv[1]
    flag = (sys.argv[2]).encode()
    print(flag)

    encrypted_flag = enc(key, flag)
    encrypted_flag = [i for i in encrypted_flag]
    print("encrypted_flag: ", encrypted_flag)

    shuffled_flag = shuffle_flag(encrypted_flag)
    print("shuffled_flag: ", shuffled_flag)

    output_o, output_s = get_output()
    instructions = get_instructions()
    print("output_o: ", output_o)
    print("output_s: ", output_s)
    # print("instructions: ", instructions)

    stack = rev_vm(output_o, output_s, instructions)
    print("stack: ", stack)
    print(shuffled_flag)
    assert(stack == shuffled_flag)
    print("[!] Success")
    ori_encrypted_flag = rev_shuffle_flag(shuffled_flag)
    ori_encrypted_flag = b"".join([bytes([fl]) for fl in ori_encrypted_flag])
    print("ori_encrypted_flag: ", ori_encrypted_flag)

    ori_flag = dec(key, ori_encrypted_flag)
    print("ori_flag: ", ori_flag)



if __name__=='__main__':
    main()