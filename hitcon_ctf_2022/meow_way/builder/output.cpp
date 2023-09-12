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



auto OP0((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG2
    JNE_0x18
    VAR_TRANSFER_3
    SUB_OPERATION
    "\x80\xF1\x6a"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP1((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG2
    JNE_0x18
    VAR_TRANSFER_3
    ADD_OPERATION
    "\x80\xF1\x25"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP2((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG1
    JNE_0x18
    VAR_TRANSFER_3
    SUB_OPERATION
    "\x80\xF1\xc2"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP3((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG1
    JNE_0x18
    VAR_TRANSFER_3
    ADD_OPERATION
    "\x80\xF1\xb9"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP4((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG1
    JNE_0x18
    VAR_TRANSFER_3
    SUB_OPERATION
    "\x80\xF1\x95"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP5((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG1
    JNE_0x18
    VAR_TRANSFER_3
    ADD_OPERATION
    "\x80\xF1\x09"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP6((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG1
    JNE_0x18
    VAR_TRANSFER_3
    SUB_OPERATION
    "\x80\xF1\x45"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP7((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG2
    JNE_0x18
    VAR_TRANSFER_3
    SUB_OPERATION
    "\x80\xF1\x90"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP8((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG2
    JNE_0x18
    VAR_TRANSFER_3
    ADD_OPERATION
    "\x80\xF1\xf6"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP9((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG2
    JNE_0x18
    VAR_TRANSFER_3
    ADD_OPERATION
    "\x80\xF1\x62"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP10((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG2
    JNE_0x18
    VAR_TRANSFER_3
    ADD_OPERATION
    "\x80\xF1\xac"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP11((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG1
    JNE_0x18
    VAR_TRANSFER_3
    SUB_OPERATION
    "\x80\xF1\x7f"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP12((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG1
    JNE_0x18
    VAR_TRANSFER_3
    ADD_OPERATION
    "\x80\xF1\xce"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP13((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG1
    JNE_0x18
    VAR_TRANSFER_3
    SUB_OPERATION
    "\x80\xF1\xd1"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP14((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG1
    JNE_0x18
    VAR_TRANSFER_3
    ADD_OPERATION
    "\x80\xF1\xa2"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP15((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG1
    JNE_0x18
    VAR_TRANSFER_3
    ADD_OPERATION
    "\x80\xF1\x77"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP16((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG1
    JNE_0x18
    VAR_TRANSFER_3
    ADD_OPERATION
    "\x80\xF1\x1c"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP17((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG2
    JNE_0x18
    VAR_TRANSFER_3
    ADD_OPERATION
    "\x80\xF1\xb1"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP18((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG1
    JNE_0x18
    VAR_TRANSFER_3
    SUB_OPERATION
    "\x80\xF1\x78"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP19((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG2
    JNE_0x18
    VAR_TRANSFER_3
    ADD_OPERATION
    "\x80\xF1\x4e"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP20((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG2
    JNE_0x18
    VAR_TRANSFER_3
    ADD_OPERATION
    "\x80\xF1\x01"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP21((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG1
    JNE_0x18
    VAR_TRANSFER_3
    SUB_OPERATION
    "\x80\xF1\x16"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP22((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG2
    JNE_0x18
    VAR_TRANSFER_3
    ADD_OPERATION
    "\x80\xF1\xdc"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP23((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG2
    JNE_0x18
    VAR_TRANSFER_3
    SUB_OPERATION
    "\x80\xF1\x82"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP24((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG1
    JNE_0x18
    VAR_TRANSFER_3
    SUB_OPERATION
    "\x80\xF1\x59"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP25((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG1
    JNE_0x18
    VAR_TRANSFER_3
    ADD_OPERATION
    "\x80\xF1\x3e"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP26((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG2
    JNE_0x18
    VAR_TRANSFER_3
    ADD_OPERATION
    "\x80\xF1\xcc"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP27((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG1
    JNE_0x18
    VAR_TRANSFER_3
    SUB_OPERATION
    "\x80\xF1\xf9"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP28((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG2
    JNE_0x18
    VAR_TRANSFER_3
    SUB_OPERATION
    "\x80\xF1\xad"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP29((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG2
    JNE_0x18
    VAR_TRANSFER_3
    SUB_OPERATION
    "\x80\xF1\x94"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP30((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG1
    JNE_0x18
    VAR_TRANSFER_3
    SUB_OPERATION
    "\x80\xF1\xd0"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP31((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG1
    JNE_0x18
    VAR_TRANSFER_3
    SUB_OPERATION
    "\x80\xF1\x22"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP32((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG2
    JNE_0x18
    VAR_TRANSFER_3
    ADD_OPERATION
    "\x80\xF1\xdf"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP33((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG2
    JNE_0x18
    VAR_TRANSFER_3
    ADD_OPERATION
    "\x80\xF1\xa1"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP34((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG2
    JNE_0x18
    VAR_TRANSFER_3
    SUB_OPERATION
    "\x80\xF1\x1e"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP35((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG2
    JNE_0x18
    VAR_TRANSFER_3
    SUB_OPERATION
    "\x80\xF1\xed"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP36((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG1
    JNE_0x18
    VAR_TRANSFER_3
    ADD_OPERATION
    "\x80\xF1\x3f"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP37((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG1
    JNE_0x18
    VAR_TRANSFER_3
    SUB_OPERATION
    "\x80\xF1\x61"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP38((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG1
    JNE_0x18
    VAR_TRANSFER_3
    SUB_OPERATION
    "\x80\xF1\xf5"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP39((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG2
    JNE_0x18
    VAR_TRANSFER_3
    ADD_OPERATION
    "\x80\xF1\xc2"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP40((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG2
    JNE_0x18
    VAR_TRANSFER_3
    SUB_OPERATION
    "\x80\xF1\x56"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP41((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG1
    JNE_0x18
    VAR_TRANSFER_3
    SUB_OPERATION
    "\x80\xF1\xa7"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP42((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG1
    JNE_0x18
    VAR_TRANSFER_3
    SUB_OPERATION
    "\x80\xF1\xa7"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP43((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG2
    JNE_0x18
    VAR_TRANSFER_3
    SUB_OPERATION
    "\x80\xF1\xd1"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP44((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG1
    JNE_0x18
    VAR_TRANSFER_3
    ADD_OPERATION
    "\x80\xF1\x7d"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP45((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG1
    JNE_0x18
    VAR_TRANSFER_3
    SUB_OPERATION
    "\x80\xF1\x8d"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP46((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG1
    JNE_0x18
    VAR_TRANSFER_3
    ADD_OPERATION
    "\x80\xF1\x61"
    STORE_RESULT
    HEAVEN_EXIT
));


auto OP47((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
    HEAVEN_ENTRY
    ANTI_DEBUG2
    JNE_0x18
    VAR_TRANSFER_3
    ADD_OPERATION
    "\x80\xF1\xf8"
    STORE_RESULT
    HEAVEN_EXIT
));

UCHAR secret[0x30] = {0x51, 0x99, 0x45, 0xdc, 0x75, 0x6a, 0xa9, 0x93, 0x98, 0x88, 0x97, 0x4b, 0x2b, 0x93, 0x96, 0xc1, 0x40, 0xcb, 0x5b, 0x5b, 0xb1, 0x9c, 0xb4, 0xe7, 0x2, 0x0, 0x43, 0x28, 0x8d, 0x67, 0xeb, 0xeb, 0x1e, 0x29, 0xee, 0xb, 0x8a, 0xd3, 0x47, 0x84, 0x21, 0x30, 0xc9, 0x8b, 0x4f, 0xc8, 0x6f, 0x26};

int main(int argc, char* argv[]) {
    char* buf = 0;
    ULONG64 ret = -1;

    if (argc < 2) {
        printf("Usage: %s <flag>\n", argv[0]);
        exit(1);
    }
    if (strlen(argv[1]) != 48) {
        printf("Wrong length\n");
        exit(1);
    }
    buf = argv[1];
    
	OP0((ULONG64)buf, (ULONG64)buf, 0xa3, (ULONG64)&ret);
	buf++;
	OP1((ULONG64)buf, (ULONG64)buf, 0x53, (ULONG64)&ret);
	buf++;
	OP2((ULONG64)buf, (ULONG64)buf, 0xfb, (ULONG64)&ret);
	buf++;
	OP3((ULONG64)buf, (ULONG64)buf, 0x2, (ULONG64)&ret);
	buf++;
	OP4((ULONG64)buf, (ULONG64)buf, 0x4f, (ULONG64)&ret);
	buf++;
	OP5((ULONG64)buf, (ULONG64)buf, 0xf5, (ULONG64)&ret);
	buf++;
	OP6((ULONG64)buf, (ULONG64)buf, 0x67, (ULONG64)&ret);
	buf++;
	OP7((ULONG64)buf, (ULONG64)buf, 0x62, (ULONG64)&ret);
	buf++;
	OP8((ULONG64)buf, (ULONG64)buf, 0xf, (ULONG64)&ret);
	buf++;
	OP9((ULONG64)buf, (ULONG64)buf, 0x8b, (ULONG64)&ret);
	buf++;
	OP10((ULONG64)buf, (ULONG64)buf, 0x4, (ULONG64)&ret);
	buf++;
	OP11((ULONG64)buf, (ULONG64)buf, 0x89, (ULONG64)&ret);
	buf++;
	OP12((ULONG64)buf, (ULONG64)buf, 0xb0, (ULONG64)&ret);
	buf++;
	OP13((ULONG64)buf, (ULONG64)buf, 0x96, (ULONG64)&ret);
	buf++;
	OP14((ULONG64)buf, (ULONG64)buf, 0xd5, (ULONG64)&ret);
	buf++;
	OP15((ULONG64)buf, (ULONG64)buf, 0x82, (ULONG64)&ret);
	buf++;
	OP16((ULONG64)buf, (ULONG64)buf, 0xfd, (ULONG64)&ret);
	buf++;
	OP17((ULONG64)buf, (ULONG64)buf, 0x27, (ULONG64)&ret);
	buf++;
	OP18((ULONG64)buf, (ULONG64)buf, 0x54, (ULONG64)&ret);
	buf++;
	OP19((ULONG64)buf, (ULONG64)buf, 0xa8, (ULONG64)&ret);
	buf++;
	OP20((ULONG64)buf, (ULONG64)buf, 0x40, (ULONG64)&ret);
	buf++;
	OP21((ULONG64)buf, (ULONG64)buf, 0xd3, (ULONG64)&ret);
	buf++;
	OP22((ULONG64)buf, (ULONG64)buf, 0x23, (ULONG64)&ret);
	buf++;
	OP23((ULONG64)buf, (ULONG64)buf, 0xc4, (ULONG64)&ret);
	buf++;
	OP24((ULONG64)buf, (ULONG64)buf, 0xd3, (ULONG64)&ret);
	buf++;
	OP25((ULONG64)buf, (ULONG64)buf, 0xfc, (ULONG64)&ret);
	buf++;
	OP26((ULONG64)buf, (ULONG64)buf, 0x59, (ULONG64)&ret);
	buf++;
	OP27((ULONG64)buf, (ULONG64)buf, 0x30, (ULONG64)&ret);
	buf++;
	OP28((ULONG64)buf, (ULONG64)buf, 0x6d, (ULONG64)&ret);
	buf++;
	OP29((ULONG64)buf, (ULONG64)buf, 0x33, (ULONG64)&ret);
	buf++;
	OP30((ULONG64)buf, (ULONG64)buf, 0x82, (ULONG64)&ret);
	buf++;
	OP31((ULONG64)buf, (ULONG64)buf, 0xfa, (ULONG64)&ret);
	buf++;
	OP32((ULONG64)buf, (ULONG64)buf, 0x7e, (ULONG64)&ret);
	buf++;
	OP33((ULONG64)buf, (ULONG64)buf, 0x29, (ULONG64)&ret);
	buf++;
	OP34((ULONG64)buf, (ULONG64)buf, 0x24, (ULONG64)&ret);
	buf++;
	OP35((ULONG64)buf, (ULONG64)buf, 0x45, (ULONG64)&ret);
	buf++;
	OP36((ULONG64)buf, (ULONG64)buf, 0x48, (ULONG64)&ret);
	buf++;
	OP37((ULONG64)buf, (ULONG64)buf, 0xf7, (ULONG64)&ret);
	buf++;
	OP38((ULONG64)buf, (ULONG64)buf, 0xe2, (ULONG64)&ret);
	buf++;
	OP39((ULONG64)buf, (ULONG64)buf, 0xcf, (ULONG64)&ret);
	buf++;
	OP40((ULONG64)buf, (ULONG64)buf, 0xd6, (ULONG64)&ret);
	buf++;
	OP41((ULONG64)buf, (ULONG64)buf, 0xee, (ULONG64)&ret);
	buf++;
	OP42((ULONG64)buf, (ULONG64)buf, 0xae, (ULONG64)&ret);
	buf++;
	OP43((ULONG64)buf, (ULONG64)buf, 0xd3, (ULONG64)&ret);
	buf++;
	OP44((ULONG64)buf, (ULONG64)buf, 0xd3, (ULONG64)&ret);
	buf++;
	OP45((ULONG64)buf, (ULONG64)buf, 0xa4, (ULONG64)&ret);
	buf++;
	OP46((ULONG64)buf, (ULONG64)buf, 0xaf, (ULONG64)&ret);
	buf++;
	OP47((ULONG64)buf, (ULONG64)buf, 0x61, (ULONG64)&ret);

    if (memcmp(secret, argv[1], 0x30) != 0)
    {
        printf("Wrong\n");
        exit(-1);
    }
    printf("I know you know the flag!\n");

    return 0;
}
