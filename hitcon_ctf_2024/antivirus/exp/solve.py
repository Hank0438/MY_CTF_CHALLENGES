from pwn import *

# Length of print_flag.exe is 0x18c.
# print_flag.exe is initialized with 'MZ' + '\x00' * 0x18a
filedata = b'MZ' + b'\x00'*0x18a
with open('samples/fuzz_pe.exe', 'wb') as f:
    f.write(filedata)

# Brute force each byte in print_flag.
index = 0
while index < 0x18b:
    # Get the output of the modified clamscan.
    # Ref: https://hxp.io/blog/94/SECCON-CTF-2022-Quals/
    '''
    -            DEFINE_ICMPOP(OP_BC_ICMP_EQ, res = (op0 == op1));
    +            DEFINE_ICMPOP(OP_BC_ICMP_EQ, printf("%d: %x =?= %x\n", bb_inst, op0, op1);res = (op0 == op1));
    -            DEFINE_ICMPOP(OP_BC_ICMP_ULT, res = (op0 < op1));
    +            DEFINE_ICMPOP(OP_BC_ICMP_ULT, printf("%d: %llx <? %llx\n", bb_inst, op0, op1);res = (op0 < op1));
    '''
    p = process(['/home/hank/clamav_build/clamav/build2/clamscan/clamscan', '--bytecode-unsigned', '-d', 'print_flag.cbc', 'samples/fuzz_pe.exe'])
    output = p.recvall(1).split(b'\n')[-16:-14]

    # Calculate the byte by xor. 
    index = int(output[0].split(b'<?')[0].split(b':')[1], 16)
    target = int(output[1].split(b'=?=')[0].split(b':')[1], 16)
    xor = int(output[1].split(b'=?=')[1], 16)
    answer = target ^ xor
    print("output:", output)
    print(f'{index=}, {target=}, {xor=}, {answer=}')

    # Write the byte to print_flag.exe.
    filedata = filedata[:index] + bytes([answer]) + filedata[index+1:]
    open('samples/fuzz_pe.exe', 'wb').write(filedata)