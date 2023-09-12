// [Note]:
//      DEP should be disable
//      project properties->linker->advanced->dep = No
#include <stdio.h>
#include <windows.h>

#define DEBUG_HEADER "[DEBUG] "
#define DEBUG(msg) printf(DEBUG_HEADER);printf(msg)

// enter 64 bit mode
// 0:  6a 33                   push   0x33
// 2:  e8 00 00 00 00          call   0x7
// 7 : 83 04 24 05             add    DWORD PTR[esp], 0x5
// b : cb                      retf
#define HEAVEN_ENTRY "\x6a\x33\xe8\x00\x00\x00\x00\x83\x04\x24\x05\xcb"

// exit 64 bit mode
// 0:  e8 00 00 00 00          call   0x5
// 5:  c7 44 24 04 23 00 00    mov    DWORD PTR[rsp + 0x4], 0x23
// c : 00
// d : 83 04 24 0d             add    DWORD PTR[rsp], 0xd
// 11 : cb                     retf
// 12 : c3                     ret
#define HEAVEN_EXIT "\xe8\x00\x00\x00\x00\xc7\x44\x24\x04\x23\x00\x00\x00\x83\x04\x24\x0d\xcb\xc3"

// 0:  67 8b 7c 24 04          mov    edi,DWORD PTR [esp+0x4]
// 5:  67 8b 74 24 0c          mov    esi, DWORD PTR[esp + 0xc]
// a : 67 8b 4c 24 14          mov    ecx, DWORD PTR[esp + 0x14]
#define VAR_TRANSFER_3 "\x67\x8B\x7C\x24\x04\x67\x8B\x74\x24\x0C\x67\x8B\x4C\x24\x14"

// IsDebuggerPresent
// 0:  48 31 c0				   xor rax, rax
// 3 : 65 48 8b 40 60          mov    rax, QWORD PTR gs : [rax + 0x60]
// 8 : 48 0f b6 40 02          movzx  rax, BYTE PTR[rax + 0x2]
// d : 67 8b 4c 24 1c          mov    ecx, DWORD PTR[esp + 0x1c]
// 12 : 67 89 01                mov    DWORD PTR[ecx], eax
// 15 : 85 c0                   test   eax, eax
#define ANTI_DEBUG1 "\x48\x31\xC0\x65\x48\x8B\x40\x60\x48\x0F\xB6\x40\x02\x67\x8B\x4C\x24\x1C\x67\x89\x01\x85\xC0"

// NtGlobalFlag
// 0:  48 31 c0					xor rax, rax
// 3 : 65 48 8b 40 60          mov    rax, QWORD PTR gs : [rax + 0x60]
// 8 : 48 0f b6 80 bc 00 00    movzx  rax, BYTE PTR[rax + 0xbc]
// f : 00
// 10 : 67 8b 4c 24 1c          mov    ecx, DWORD PTR[esp + 0x1c]
// 15 : 83 e0 70				and    eax, 0x70
// 18 : 67 89 01                mov    DWORD PTR[ecx], eax
// 1b : 85 c0                   test   eax, eax
#define ANTI_DEBUG2 "\x48\x31\xC0\x65\x48\x8B\x40\x60\x48\x0F\xB6\x80\xBC\x00\x00\x00\x67\x8B\x4C\x24\x1C\x83\xE0\x70\x67\x89\x01\x85\xC0"


// add    cl, BYTE PTR[esi]
#define ADD_OPERATION "\x67\x02\x0e"

// sub    cl,BYTE PTR [esi]
#define SUB_OPERATION "\x67\x2a\x0e"

// xor    cl,BYTE PTR [esi]
#define XOR_OPERATION "\x67\x32\x0e"


// mov    BYTE PTR[edi], cl
#define STORE_RESULT "\x67\x88\x0f"

// 17: 75 12                   jne     2b <exit>
#define JNE_0x12 "\x75\x12"

// 17: 75 15                   jne     2e <exit>
#define JNE_0x15 "\x75\x15"

// 17: 75 18                   jne     31 <exit>
#define JNE_0x18 "\x75\x18"


