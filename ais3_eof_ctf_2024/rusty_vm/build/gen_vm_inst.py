#!/usr/bin/python3
import random

flag = b"flag{aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa}"
gb_list = ['POP', 'NOP']
noarg_op_list = ['XOR', 'ADD', 'SUB', 'NOT', 'DUP', 'SWAP', 'ROT', 'OVER']
arg1_op_list = ['PUSH']
stack1_op_list = ['ROL', 'ROR']
branch_op_list = ['JE', 'JNE']
vm_code_list = []

PUSH = 0x01 #=> self.instruction_push(),
POP = 0x02 #=> self.instruction_pop(),
ADD = 0x03 #=> self.instruction_add(),
SUB = 0x04 #=> self.instruction_sub(),
# 0x05 => self.instruction_mul(),
# 0x06 => self.instruction_div(),
# 0x07 => self.instruction_mod(),
# AND = 0x08 #=> self.instruction_and(),
# OR = 0x09 #=> self.instruction_or(),
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
# 0x16 => self.instruction_branch_if_negative(),
NOP = 0x17 #=> self.instruction_nop(),

f = None

mapping = {
    'PUSH': PUSH,
    'POP': POP,
    'ADD': ADD,
    'SUB': SUB,
    'XOR': XOR,
    'NOT': NOT,
    'ROL': ROL,
    'ROR': ROR,
    'DUP': DUP,
    'SWAP': SWAP,
    'ROT': ROT,
    'OVER': OVER,
    'HALT': HALT,
    'PUTS': PUTS,
    'NOP': NOP,
    'JE': JE,
    'JNE': JNE,
}

def oper(opcode, f, k):
    new_val = None
    if opcode == "XOR":
        new_val = f ^ k
    elif opcode == "ADD":
        new_val = f + k
    elif opcode == "SUB":
        new_val = f - k
    return new_val


def init_vm_inst():
    global f
    f = open("vm_instructions.txt", "w")
    f.write("\tlet bytecode_program = vec![\n")

def write_vm_inst(opcode, arg):
    global f
    if arg != None:
        f.write(f"\t\t{hex(mapping[opcode])}, {hex(arg)}, \t// {opcode} {hex(arg)}\n")
    else:
        f.write(f"\t\t{hex(mapping[opcode])}, \t// {opcode}\n")
    
def close_vm_inst():
    global f
    f.write("\t];\n")
    f.close()
    f = None

def gen_vm_code():
    add_cnt = 0
    sub_cnt = 0
    je_cnt = 0
    jne_cnt = 0

    init_vm_inst()
    encrypted_flag = [f for f in flag]
    op_list = noarg_op_list + arg1_op_list + stack1_op_list
    for i in range(300):
        samples = random.sample(range(len(encrypted_flag)), 2)
        src_id, dst_id = samples[0], samples[1]
        rand = random.choice(range(0x100))
        offset = random.choice(range(0x8))
        if (rand % 2) == 1:
            opcode = "POP"
        elif (rand % 71) == 1:
            opcode = 'JE'
        elif (rand % 67) == 1:
            opcode = 'JNE'
        else:
            opcode = random.choice(op_list)

        # make some inst fewer
        if opcode == 'SUB':
            if sub_cnt >= 2:
                continue
            sub_cnt += 1

        if opcode == 'ADD':
            if add_cnt >= 2:
                continue
            add_cnt += 1

        if opcode in branch_op_list:
            write_vm_inst("PUSH", rand)
            write_vm_inst(opcode, offset)
            for _ in range(offset):
                noarg_opcode = random.choice(noarg_op_list + gb_list)
                write_vm_inst(noarg_opcode, None)
        elif opcode in arg1_op_list:
            write_vm_inst(opcode, rand)
        elif opcode in (stack1_op_list + noarg_op_list):
            write_vm_inst("PUSH", offset)
            write_vm_inst(opcode, None)
        else:
            write_vm_inst(opcode, None)


    close_vm_inst()

gen_vm_code()