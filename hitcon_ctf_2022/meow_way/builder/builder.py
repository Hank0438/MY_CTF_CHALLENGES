import os
import random

TEMPLATE_CPP_PATH = "template.cpp"
OUTPUT_CPP_PATH = "output.cpp"
FLAG_TXT_PATH = "flag.txt"

function_template = '''
auto OP{idx}((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG{anti_dbg_idx}
    JNE_0x18
    VAR_TRANSFER_3
    {ins}_OPERATION
    "\\x80\\xF1\\x{hardcoded_xor_val}"
    STORE_RESULT
    HEAVEN_EXIT
));

'''

main_func_upper = '''
int main(int argc, char* argv[]) {
    char* buf = 0;
    ULONG64 ret = -1;

    if (argc < 2) {
        printf("Usage: %s <flag>\\n", argv[0]);
        exit(1);
    }
    if (strlen(argv[1]) != 48) {
        printf("Wrong length\\n");
        exit(1);
    }
    buf = argv[1];
    
'''
main_func_lower = '''
    if (memcmp(secret, argv[1], 0x30) != 0)
    {
        printf("Wrong\\n");
        exit(-1);
    }
    printf("I know you know the flag!\\n");

    return 0;
}
'''


def gen_main():
    
    FLAG = open(FLAG_TXT_PATH, "r").readline().strip().encode()
    # FLAG = b"hitcon{___7U5T_4_S1mpIE_xB6_M@G1C_4_mE0w_W@y___}"
    print(FLAG)
    SECRET = b""
    first_oprand_val_list = [random.randrange(0,256) for _ in range(0x30)]
    second_oprand_val_list = [random.randrange(0,256) for _ in range(0x30)]
    ins_list = [random.sample(["ADD", "SUB"], 1)[0] for _ in range(0x30)]
    anti_dbg_idx_list = [random.sample(["1", "2"], 1)[0] for _ in range(0x30)]
    oneline_code = "\tOP{idx}((ULONG64)buf, (ULONG64)buf, {val}, (ULONG64)&ret);\n"


    # Gen Functions
    function_list = ["" for _ in range(0x30)]
    for i in range(0x30):
        function_list[i] = function_template.format(
            idx=str(i),
            anti_dbg_idx=anti_dbg_idx_list[i],
            ins=ins_list[i],
            hardcoded_xor_val=hex(second_oprand_val_list[i])[2:].rjust(2, "0")
            )


    # Gen Secret
    for i in range(0x30):
        s = 0
        if ins_list[i] == "ADD":
            s = (first_oprand_val_list[i] + FLAG[i]) & 0xff
        if ins_list[i] == "SUB":
            s = (first_oprand_val_list[i] - FLAG[i]) & 0xff
        s ^= second_oprand_val_list[i]
        SECRET += bytes([s])
    hex_sequence = "{" + ", ".join([hex(s) for s in SECRET]) + "}"
    print(hex_sequence)
    # print([hex(i) for i in first_oprand_val_list])
    # print([hex(i) for i in second_oprand_val_list])
    # print(ins_list)


    
    template = open(TEMPLATE_CPP_PATH, "r").readlines()
    lnidx_func_op_declaration = -1
    lnidx_var_secret_declaration = -1
    lnidx_code = -1

    
    # for idx, line in enumerate(template):
    #     if "<FILL OPERATIONS DECLARATION HERE>" in line: 
    #         lnidx_func_op_declaration = idx
    #     if "<FILL SECRET DECLARATION HERE>" in line: 
    #         lnidx_var_secret_declaration = idx
    #     if "<FILL OPERATIONS HERE>" in line:
    #         lnidx_code = idx

    # template[lnidx_var_secret_declaration] = f"UCHAR secret[0x30] = {hex_sequence};\n"

    # template.pop(lnidx_code)

    # Gen Code
    f_output = open(OUTPUT_CPP_PATH, "w")
    for line in template:
        f_output.write(line)

    ## Write Declaration of Operations
    for i in range(0x30):
        f_output.write(function_list[i])

    f_output.write(f"UCHAR secret[0x30] = {hex_sequence};\n")

    f_output.write(main_func_upper)

    ## Write Operations
    for i in range(0x30):
        f_output.write(oneline_code.format(idx=str(i), val=hex(first_oprand_val_list[i])))
    
        if i < 0x2f:
            f_output.write("\tbuf++;\n")

    f_output.write(main_func_lower)


gen_main()


