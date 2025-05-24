#!/usr/bin/python3
from subprocess import Popen, PIPE, STDOUT

flag_list = []
with open("flags.txt", "r") as f:
    flag_list = [ flg.strip() for flg in f.readlines()[:1001] ]


p = Popen(['diff', './rusty_vm_output/0', './rusty_vm_output/default'], stdout=PIPE, stdin=PIPE, stderr=PIPE)
stdout_data = p.communicate()[0]
print(stdout_data)
print(len(stdout_data))


for idx, flag in enumerate(flag_list):
    p = Popen(['diff', f'./rusty_vm_output/{str(idx)}', './rusty_vm_output/0'], stdout=PIPE, stdin=PIPE, stderr=PIPE)
    stdout_data = p.communicate()[0]
    if len(stdout_data) == 0:
        print("the same")
        print(idx, flag)

    