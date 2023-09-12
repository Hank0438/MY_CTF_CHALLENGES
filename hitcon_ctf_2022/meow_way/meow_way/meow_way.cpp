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
	ANTI_DEBUG1
	JNE_0x18
	VAR_TRANSFER_3
	SUB_OPERATION
	"\x80\xF1\x9e"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP1((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG2
	JNE_0x18
	VAR_TRANSFER_3
	SUB_OPERATION
	"\x80\xF1\xec"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP2((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG1
	JNE_0x18
	VAR_TRANSFER_3
	ADD_OPERATION
	"\x80\xF1\xa4"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP3((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG2
	JNE_0x18
	VAR_TRANSFER_3
	ADD_OPERATION
	"\x80\xF1\x69"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP4((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG2
	JNE_0x18
	VAR_TRANSFER_3
	ADD_OPERATION
	"\x80\xF1\x85"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP5((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG2
	JNE_0x18
	VAR_TRANSFER_3
	SUB_OPERATION
	"\x80\xF1\xdb"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP6((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG1
	JNE_0x18
	VAR_TRANSFER_3
	ADD_OPERATION
	"\x80\xF1\x4d"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP7((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG1
	JNE_0x18
	VAR_TRANSFER_3
	SUB_OPERATION
	"\x80\xF1\xc9"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP8((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG2
	JNE_0x18
	VAR_TRANSFER_3
	ADD_OPERATION
	"\x80\xF1\x09"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP9((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG2
	JNE_0x18
	VAR_TRANSFER_3
	ADD_OPERATION
	"\x80\xF1\x51"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP10((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG2
	JNE_0x18
	VAR_TRANSFER_3
	ADD_OPERATION
	"\x80\xF1\xb3"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP11((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG1
	JNE_0x18
	VAR_TRANSFER_3
	ADD_OPERATION
	"\x80\xF1\xec"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP12((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG2
	JNE_0x18
	VAR_TRANSFER_3
	SUB_OPERATION
	"\x80\xF1\xd0"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP13((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG2
	JNE_0x18
	VAR_TRANSFER_3
	SUB_OPERATION
	"\x80\xF1\x61"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP14((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG2
	JNE_0x18
	VAR_TRANSFER_3
	ADD_OPERATION
	"\x80\xF1\x18"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP15((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG2
	JNE_0x18
	VAR_TRANSFER_3
	ADD_OPERATION
	"\x80\xF1\xd5"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP16((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG1
	JNE_0x18
	VAR_TRANSFER_3
	SUB_OPERATION
	"\x80\xF1\x2a"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP17((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG2
	JNE_0x18
	VAR_TRANSFER_3
	ADD_OPERATION
	"\x80\xF1\xc8"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP18((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG2
	JNE_0x18
	VAR_TRANSFER_3
	SUB_OPERATION
	"\x80\xF1\x16"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP19((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG1
	JNE_0x18
	VAR_TRANSFER_3
	SUB_OPERATION
	"\x80\xF1\x2a"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP20((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG1
	JNE_0x18
	VAR_TRANSFER_3
	SUB_OPERATION
	"\x80\xF1\x66"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP21((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG1
	JNE_0x18
	VAR_TRANSFER_3
	SUB_OPERATION
	"\x80\xF1\xf6"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP22((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG2
	JNE_0x18
	VAR_TRANSFER_3
	ADD_OPERATION
	"\x80\xF1\x8e"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP23((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG2
	JNE_0x18
	VAR_TRANSFER_3
	ADD_OPERATION
	"\x80\xF1\xde"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP24((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG1
	JNE_0x18
	VAR_TRANSFER_3
	ADD_OPERATION
	"\x80\xF1\x4b"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP25((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG1
	JNE_0x18
	VAR_TRANSFER_3
	ADD_OPERATION
	"\x80\xF1\x79"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP26((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG1
	JNE_0x18
	VAR_TRANSFER_3
	SUB_OPERATION
	"\x80\xF1\xad"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP27((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG1
	JNE_0x18
	VAR_TRANSFER_3
	SUB_OPERATION
	"\x80\xF1\xd9"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP28((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG1
	JNE_0x18
	VAR_TRANSFER_3
	SUB_OPERATION
	"\x80\xF1\x18"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP29((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG1
	JNE_0x18
	VAR_TRANSFER_3
	ADD_OPERATION
	"\x80\xF1\x26"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP30((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG2
	JNE_0x18
	VAR_TRANSFER_3
	SUB_OPERATION
	"\x80\xF1\xbb"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP31((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG2
	JNE_0x18
	VAR_TRANSFER_3
	SUB_OPERATION
	"\x80\xF1\xe1"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP32((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG2
	JNE_0x18
	VAR_TRANSFER_3
	ADD_OPERATION
	"\x80\xF1\xf3"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP33((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG1
	JNE_0x18
	VAR_TRANSFER_3
	ADD_OPERATION
	"\x80\xF1\xef"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP34((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG2
	JNE_0x18
	VAR_TRANSFER_3
	ADD_OPERATION
	"\x80\xF1\x8f"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP35((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG1
	JNE_0x18
	VAR_TRANSFER_3
	ADD_OPERATION
	"\x80\xF1\xc8"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP36((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG2
	JNE_0x18
	VAR_TRANSFER_3
	ADD_OPERATION
	"\x80\xF1\x6d"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP37((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG1
	JNE_0x18
	VAR_TRANSFER_3
	ADD_OPERATION
	"\x80\xF1\x3c"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP38((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG1
	JNE_0x18
	VAR_TRANSFER_3
	SUB_OPERATION
	"\x80\xF1\x28"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP39((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG2
	JNE_0x18
	VAR_TRANSFER_3
	SUB_OPERATION
	"\x80\xF1\xfe"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP40((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG1
	JNE_0x18
	VAR_TRANSFER_3
	SUB_OPERATION
	"\x80\xF1\x42"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP41((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG2
	JNE_0x18
	VAR_TRANSFER_3
	SUB_OPERATION
	"\x80\xF1\xce"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP42((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG2
	JNE_0x18
	VAR_TRANSFER_3
	ADD_OPERATION
	"\x80\xF1\x84"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP43((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG1
	JNE_0x18
	VAR_TRANSFER_3
	ADD_OPERATION
	"\x80\xF1\xb2"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP44((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG2
	JNE_0x18
	VAR_TRANSFER_3
	SUB_OPERATION
	"\x80\xF1\x05"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP45((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG2
	JNE_0x18
	VAR_TRANSFER_3
	SUB_OPERATION
	"\x80\xF1\xa0"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP46((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG1
	JNE_0x18
	VAR_TRANSFER_3
	ADD_OPERATION
	"\x80\xF1\xaf"
	STORE_RESULT
	HEAVEN_EXIT
	));


auto OP47((int(cdecl*)(ULONG64, ULONG64, ULONG64, ULONG64))((PCSTR)
	HEAVEN_ENTRY
	ANTI_DEBUG1
	JNE_0x18
	VAR_TRANSFER_3
	ADD_OPERATION
	"\x80\xF1\x9d"
	STORE_RESULT
	HEAVEN_EXIT
	));

UCHAR secret[0x30] = { 0x71, 0x2a, 0xf0, 0x39, 0xbf, 0x15, 0xf5, 0xb2, 0xdc, 0xa2, 0xc6, 0x5e, 0x92, 0x6b, 0x46, 0xd6, 0x8, 0x9b, 0xcb, 0xb9, 0x2e, 0x4, 0x1, 0xc5, 0xd7, 0x78, 0x87, 0x9, 0x94, 0xf7, 0x93, 0xcf, 0x7c, 0xe3, 0xd2, 0x21, 0x87, 0xcf, 0x69, 0xd0, 0xaa, 0xd0, 0xfb, 0xcc, 0x9a, 0xd5, 0x1e, 0xd7 };

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

	OP0((ULONG64)buf, (ULONG64)buf, 0x57, (ULONG64)&ret);
	buf++;
	OP1((ULONG64)buf, (ULONG64)buf, 0x2f, (ULONG64)&ret);
	buf++;
	OP2((ULONG64)buf, (ULONG64)buf, 0xe0, (ULONG64)&ret);
	buf++;
	OP3((ULONG64)buf, (ULONG64)buf, 0xed, (ULONG64)&ret);
	buf++;
	OP4((ULONG64)buf, (ULONG64)buf, 0xcb, (ULONG64)&ret);
	buf++;
	OP5((ULONG64)buf, (ULONG64)buf, 0x3c, (ULONG64)&ret);
	buf++;
	OP6((ULONG64)buf, (ULONG64)buf, 0x3d, (ULONG64)&ret);
	buf++;
	OP7((ULONG64)buf, (ULONG64)buf, 0xda, (ULONG64)&ret);
	buf++;
	OP8((ULONG64)buf, (ULONG64)buf, 0x76, (ULONG64)&ret);
	buf++;
	OP9((ULONG64)buf, (ULONG64)buf, 0x94, (ULONG64)&ret);
	buf++;
	OP10((ULONG64)buf, (ULONG64)buf, 0x3e, (ULONG64)&ret);
	buf++;
	OP11((ULONG64)buf, (ULONG64)buf, 0x5d, (ULONG64)&ret);
	buf++;
	OP12((ULONG64)buf, (ULONG64)buf, 0x77, (ULONG64)&ret);
	buf++;
	OP13((ULONG64)buf, (ULONG64)buf, 0x5e, (ULONG64)&ret);
	buf++;
	OP14((ULONG64)buf, (ULONG64)buf, 0xff, (ULONG64)&ret);
	buf++;
	OP15((ULONG64)buf, (ULONG64)buf, 0xcf, (ULONG64)&ret);
	buf++;
	OP16((ULONG64)buf, (ULONG64)buf, 0x81, (ULONG64)&ret);
	buf++;
	OP17((ULONG64)buf, (ULONG64)buf, 0x0, (ULONG64)&ret);
	buf++;
	OP18((ULONG64)buf, (ULONG64)buf, 0xe, (ULONG64)&ret);
	buf++;
	OP19((ULONG64)buf, (ULONG64)buf, 0x0, (ULONG64)&ret);
	buf++;
	OP20((ULONG64)buf, (ULONG64)buf, 0xb8, (ULONG64)&ret);
	buf++;
	OP21((ULONG64)buf, (ULONG64)buf, 0x3b, (ULONG64)&ret);
	buf++;
	OP22((ULONG64)buf, (ULONG64)buf, 0x4a, (ULONG64)&ret);
	buf++;
	OP23((ULONG64)buf, (ULONG64)buf, 0xbc, (ULONG64)&ret);
	buf++;
	OP24((ULONG64)buf, (ULONG64)buf, 0x24, (ULONG64)&ret);
	buf++;
	OP25((ULONG64)buf, (ULONG64)buf, 0xbf, (ULONG64)&ret);
	buf++;
	OP26((ULONG64)buf, (ULONG64)buf, 0x60, (ULONG64)&ret);
	buf++;
	OP27((ULONG64)buf, (ULONG64)buf, 0x2f, (ULONG64)&ret);
	buf++;
	OP28((ULONG64)buf, (ULONG64)buf, 0xd9, (ULONG64)&ret);
	buf++;
	OP29((ULONG64)buf, (ULONG64)buf, 0x91, (ULONG64)&ret);
	buf++;
	OP30((ULONG64)buf, (ULONG64)buf, 0x6f, (ULONG64)&ret);
	buf++;
	OP31((ULONG64)buf, (ULONG64)buf, 0x5f, (ULONG64)&ret);
	buf++;
	OP32((ULONG64)buf, (ULONG64)buf, 0x4c, (ULONG64)&ret);
	buf++;
	OP33((ULONG64)buf, (ULONG64)buf, 0xad, (ULONG64)&ret);
	buf++;
	OP34((ULONG64)buf, (ULONG64)buf, 0x29, (ULONG64)&ret);
	buf++;
	OP35((ULONG64)buf, (ULONG64)buf, 0x8a, (ULONG64)&ret);
	buf++;
	OP36((ULONG64)buf, (ULONG64)buf, 0x7d, (ULONG64)&ret);
	buf++;
	OP37((ULONG64)buf, (ULONG64)buf, 0xae, (ULONG64)&ret);
	buf++;
	OP38((ULONG64)buf, (ULONG64)buf, 0x71, (ULONG64)&ret);
	buf++;
	OP39((ULONG64)buf, (ULONG64)buf, 0xa5, (ULONG64)&ret);
	buf++;
	OP40((ULONG64)buf, (ULONG64)buf, 0x47, (ULONG64)&ret);
	buf++;
	OP41((ULONG64)buf, (ULONG64)buf, 0x75, (ULONG64)&ret);
	buf++;
	OP42((ULONG64)buf, (ULONG64)buf, 0x3f, (ULONG64)&ret);
	buf++;
	OP43((ULONG64)buf, (ULONG64)buf, 0x5, (ULONG64)&ret);
	buf++;
	OP44((ULONG64)buf, (ULONG64)buf, 0xfe, (ULONG64)&ret);
	buf++;
	OP45((ULONG64)buf, (ULONG64)buf, 0xd4, (ULONG64)&ret);
	buf++;
	OP46((ULONG64)buf, (ULONG64)buf, 0x52, (ULONG64)&ret);
	buf++;
	OP47((ULONG64)buf, (ULONG64)buf, 0xcd, (ULONG64)&ret);

	if (memcmp(secret, argv[1], 0x30) != 0)
	{
		printf("Wrong\n");
		exit(-1);
	}
	printf("I know you know the flag!\n");

	return 0;
}