#!/usr/bin/python3
import os
import random
import sys

with open('/home/user/flag.txt', "r") as f:
    flag = f.read()

def start():
    random.seed(os.urandom(32))
    num_list = []
    for i in range(624):
        rand = random.getrandbits(32)
        num_list.append(rand)
    
    print("Oh! Great one who summons me, I stand by my oath: loyalty to wishes 3 !")
    
    first = int(input("[>] Give me your first wish: "))
    print("[>] Here is the first number: %d" % num_list[first])
    sys.stdout.flush()
    
    second = int(input("[>] Give me your second wish: "))
    print("[>] Here is the second number: %d" % num_list[second])
    sys.stdout.flush()

    third = int(input("[>] Give me your third wish: "))
    if num_list[third] < 1000:
        print("[>] Here is the third number: %d" % num_list[third])
    else:
        print("[>] Bad luck :(")       
    sys.stdout.flush()

    guess = int(input("[>] Only my master can predict the next random: "))
    next = random.getrandbits(32)
    if guess == next:
        print("[>] As you wish, my master! Here is the flag %s" % flag)
    else:
        print("[>] A genie without a master, goes back in their lamp.")

start()