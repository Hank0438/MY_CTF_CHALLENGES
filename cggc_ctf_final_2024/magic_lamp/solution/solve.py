import os
import random
from cracker import RandCrack
from pwn import *

r = remote("140.114.77.172", 60002)

r.recvuntil("index: ")


r.sendline("1")
str1 = r.recvuntil("index: ").strip().decode()
str1_idx0 = str1.index("number: ")
str1_idxn = str1.index("\n")
n1 = int(str1[str1_idx0+8: str1_idxn])
print(n1)


r.sendline("397")
str397 = r.recvuntil("index: ").strip().decode()
str397_idx0 = str397.index("number: ")
str397_idxn = str397.index("\n")
n397 = int(str397[str397_idx0+8: str397_idxn])
print(n397)

r.sendline("0")
str0 = r.recvuntil("random: ").strip().decode()
#print("str0", str0)

try:
    str0_idx0 = str0.index("number: ")
    str0_idxn = str0.index("\n")
except:
    str0_idx0 = str0_idxn = -1
if str0_idx0 > -1:
    n0 = int(str0[str0_idx0+8: str0_idxn])
    print(n0)


rc = RandCrack()

#x0 = rc._to_int(rc._harden_inverse(rc._to_bitarray(n0)))
x1 = rc._to_int(rc._harden_inverse(rc._to_bitarray(n1)))
x397 = rc._to_int(rc._harden_inverse(rc._to_bitarray(n397)))
guess1 = rc._to_int(rc._harden(rc._to_bitarray(rc.twist(0x80000000, x1, x397))))
guess2 = rc._to_int(rc._harden(rc._to_bitarray(rc.twist(0x00000000, x1, x397))))

print(guess1)
print(guess2)


r.sendline(str(guess1))

r.interactive()
