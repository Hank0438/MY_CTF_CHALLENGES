import random
import string
import subprocess


op_list = ['XOR', 'ADD', 'SUB']

def gen_flag():
    return ("CGGC{" + "".join([random.choice("olvmOLVM01") for i in range(50)]) + "}").encode()

# print(gen_flag())
flag = b"CGGC{oMLoM0OOLmM10l0OLomM10L01VLLvlMM1vlVlllvMv1Vmoo1mm}"
# flag = b"flag"
fd_vm_code = None
vm_code_list = []

def init_vm_code():
    global fd_vm_code
    fd_vm_code = open("C:\\Users\\hank_k_chen\\Desktop\\ollvm-16\\vm_code.txt", "w")
    
def close_vm_code():
    fd_vm_code.close()

def save_vm_code_inst1(opcode, secret, src_id):
    vm_code = f"\t\tINST1({opcode}, {secret}, {src_id})\n"
    vm_code_list.append(vm_code)
    fd_vm_code.write(vm_code)

def save_vm_code_inst2(opcode, src_id, dst_id):
    vm_code = f"\t\tINST2({opcode}, {src_id}, {dst_id})\n"
    vm_code_list.append(vm_code)
    fd_vm_code.write(vm_code)

def save_vm_code_puts(key_len):
    for i in range(key_len):
        vm_code = "\t\t{PUTS," + str(i) + "},\n"
        vm_code_list.append(vm_code)
        fd_vm_code.write(vm_code) 

def output_encrypted_flag(encrypted_flag):
    with open("encrypted_flag.txt", "wb") as f:
        f.write(encrypted_flag)


def oper(opcode, f, k):
    new_val = None
    if opcode == "XOR":
        new_val = f ^ k
    elif opcode == "ADD":
        new_val = f + k
    elif opcode == "SUB":
        new_val = f - k
    return new_val


def gen_vm_code():
    encrypted_flag = [f for f in flag]
    for i in range(80):
        samples = random.sample(range(len(encrypted_flag)), 2)
        src_id, dst_id = samples[0], samples[1]
        rand = random.choice(range(0x100))
        opcode = random.choice(op_list)
        choice = random.choice(range(10))

        if ((choice % 2) == 1):
            new_val = oper(opcode, encrypted_flag[src_id], rand)
            if (new_val > 0xff) or (new_val < 0):
                continue
            save_vm_code_inst1(opcode, rand, src_id)
            encrypted_flag[src_id] = new_val
        else:
            new_val = oper(opcode, encrypted_flag[dst_id], encrypted_flag[src_id])
            if (new_val > 0xff) or (new_val < 0):
                continue
            save_vm_code_inst2(opcode, src_id, dst_id)
            encrypted_flag[dst_id] = new_val
    
    save_vm_code_puts(len(encrypted_flag))
    encrypted_flag = b"".join([bytes([f]) for f in encrypted_flag])
    output_encrypted_flag(encrypted_flag)
    print("[+] encrypted_flag: ", encrypted_flag)

def gen_c_code():
    template_code = open("vm_template.c", "r")
    lines = template_code.readlines()
    _id, _line = list(filter(lambda x: x[1].startswith("PUT_SOME_VM_CODE_HERE"), enumerate(lines)))[0]
    
    del lines[_id]
    for _idx, _code in enumerate(vm_code_list):
        lines.insert(_id+_idx, _code)
    release_code = open("vm.c", "w")
    for l in lines:
        release_code.write(l)

if __name__ == "__main__":
    init_vm_code()
    gen_vm_code()
    close_vm_code()
    gen_c_code()