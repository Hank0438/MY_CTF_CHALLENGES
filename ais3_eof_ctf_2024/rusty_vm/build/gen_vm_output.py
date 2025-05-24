#!/usr/bin/python3
from subprocess import Popen, PIPE, STDOUT
from shutil import copyfile

flag_list = []
with open("flags.txt", "r") as f:
    flag_list = [ flg.strip() for flg in f.readlines()[:1001] ]


for idx, flag in enumerate(flag_list):
    p = Popen(['./rust_vm/target/release/rust_vm'], stdout=PIPE, stdin=PIPE, stderr=PIPE)
    stdout_data = p.communicate(input=flag.encode())[0]

    with open("rusty_vm_output/"+str(idx), "wb") as f:
        f.write(stdout_data)

copyfile("rusty_vm_output/0", "rusty_vm_output/default")