BITS 64

                            ; DOS Header
    dw 'MZ'                 ; e_magic
    dw 0                    ; [UNUSED] e_cblp
pe_hdr:                                                 ; PE Header
    dw 'PE'                 ; [UNUSED] c_cp             ; Signature
    dw 0                    ; [UNUSED] e_crlc           ; Signature (Cont)
                                                        ; Image File Header
    dw 0x8664               ; [UNUSED] e_cparhdr        ; Machine
code:                                                                                       
    dw 0x01                 ; [UNUSED] e_minalloc       ; NumberOfSections                     
    times 14-($-code) db 0  ; [UNUSED] e_maxalloc       ; [UNUSED] TimeDateStamp
                            ; [UNUSED] e_ss             ; [UNUSED] TimeDateStamp (Cont)
                            ; [UNUSED] e_sp             ; [UNUSED] PointerToSymbolTable
                            ; [UNUSED] e_csum           ; [UNUSED] PointerToSymbolTable (Cont)
                            ; [UNUSED] e_ip             ; [UNUSED] NumberOfSymbols
                            ; [UNUSED] e_cs             ; [UNUSED] NumberOfSymbols (Cont)
    dw opt_hdr_size         ; [UNUSED] e_lfarlc         ; SizeOfOptionalHeader
    dw 0x22                 ; [UNUSED] e_ovno           ; Characteristics
opt_hdr:                                                ; Optional Header, COFF Standard Fields
    dw 0x020b               ; [UNUSED] e_res            ; Magic (PE32+)
    db 0                    ; [UNUSED] e_res (Cont)     ; [UNUSED] MajorLinkerVersion
    db 0                    ; [UNUSED] e_res (Cont)     ; [UNUSED] MinorLinkerVersion
    dd code_size            ; [UNUSED] e_res (Cont)     ; SizeOfCode
    dw 0                    ; [UNUSED] e_oemid          ; [UNUSED] SizeOfInitializedData
    dw 0                    ; [UNUSED] e_oeminfo        ; [UNUSED] SizeOfInitializedData (Cont)
    dd 0                    ; [UNUSED] e_res2           ; [UNUSED] SizeOfUninitializedData
    dd entry                ; [UNUSED] e_res2 (Cont)    ; AddressOfEntryPoint
    dd code                 ; [UNUSED] e_res2 (Cont)    ; BaseOfCode
                                                        ; Optional Header, NT Additional Fields
    dq 0x000140000000       ; [UNUSED] e_res2 (Cont)    ; ImageBase
    dd pe_hdr               ; e_lfanew                  ; [MODIFIED] SectionAlignment (0x10 -> 0x04)
    dd 0x04                                             ; [MODIFIED] FileAlignment (0x10)
    dw 0x06                                             ; [UNUSED] MajorOperatingSystemVersion
    dw 0                                                ; [UNUSED] MinorOperatingSystemVersion
    dw 0                                                ; [UNUSED] MajorImageVersion
    dw 0                                                ; [UNUSED] MinorImageVersion
    dw 0x06                                             ; MajorSubsystemVersion
    dw 0                                                ; MinorSubsystemVersion
    dd 0                                                ; [UNUSED] Reserved1
    dd file_size                                        ; SizeOfImage
    dd hdr_size                                         ; SizeOfHeaders
    dd 0                                                ; [UNUSED] CheckSum
    dw 0x03                                             ; Subsystem (Windows GUI)
    dw 0x8160                                           ; DllCharacteristics
    dq 0x100000                                         ; SizeOfStackReserve
    dq 0x1000                                           ; SizeOfStackCommit
    dq 0x100000                                         ; SizeOfHeapReserve
    dq 0x1000                                           ; SizeOfHeapCommit
    dd 0                                                ; LoaderFlags
    dd 0x2                                              ; NumberOfRvaAndSizes

; Optional Header, Data Directories
    dd 0                    ; [UNUSED] Export, RVA      
    dd 0                    ; [UNUSED] Export, Size     
    dd itbl                 ; Import, RVA               
    dd itbl_size            ; Import, Size              

opt_hdr_size equ $-opt_hdr

                            ; Section Table
    section_name db '.', 0  ; Name
    times 8-($-section_name) db 0
    dd sect_size            ; VirtualSize
    dd iatbl                ; VirtualAddress
    dd code_size            ; SizeOfRawData
    dd iatbl                ; PointerToRawData

; python3 -c "a=([hex(ord(i)^0xc8) for i in 'hitcon{__secret_in_clamav_bytecode_signature__}']);print(len(a), ','.join(a))"
content:          ; Strings
    db 0xa0,0xa1,0xbc,0xab,0xa7,0xa6,0xb3,0x97
    db 0x97,0xbb,0xad,0xab,0xba,0xad,0xbc,0x97
    db 0xa1,0xa6,0x97,0xab,0xa4,0xa9,0xa5,0xa9
    db 0xbe,0x97,0xaa,0xb1,0xbc,0xad,0xab,0xa7
    db 0xac,0xad,0x97,0xbb,0xa1,0xaf,0xa6,0xa9
    db 0xbc,0xbd,0xba,0xad,0x97,0x97,0xb5,0xc8
                            ; [UNUSED] PointerToRelocations
                            ; [UNUSED] PointerToLinenumbers
                            ; [UNUSED] NumberOfRelocations
                            ; [UNUSED] NumberOfLinenumbers
                            ; [UNUSED] Characteristics
hdr_size equ $-$$

; Symbol
symbol_GetStdHandle:
    dw 0x0294               ; [UNUSED] Function Order
    db 'GetStdHandle', 0     ; Function Name
symbol_WriteConsoleA:
    dw 0x0295               ; [UNUSED] Function Order
    db 'WriteConsoleA', 0     ; Function Name
dll_name:
    db 'KERNEL32.dll', 0
    db 0

iatbl:                                   ; Import Address Directory
iatbl_GetStdHandle:
    dq symbol_GetStdHandle               ; [USEDAFTERLOAD] DLLFuncEntry (GetStdHandle)
iatbl_WriteConsoleA:
    dq symbol_WriteConsoleA              ; [USEDAFTERLOAD] DLLFuncEntry (WriteConsoleA)
iatbl_size equ $-iatbl

; Entry
entry:
    mov ecx, 0FFFFFFF5h         ; StdHandle
    call [rel iatbl_GetStdHandle]    ; GetStdHandle
    xor r9d, r9d                ; lpNumberOfCharsWritten
    lea r8d, [r9+30h]           ; nNumberOfCharsToWrite
    lea rdx, [rel content]      ; lpBuffer
    xor rcx, rcx
decrypt:
    xor BYTE[rdx+rcx], 0xc8
    inc rcx
    cmp rcx, 30h
    jne decrypt 
    lea rdx, [rel content]      ; lpBuffer
    mov rcx, rax                ; hConsoleOutput
    call [rel iatbl_WriteConsoleA]     ; WriteConsoleA
    ret

itbl:                       ; Import Directory
    dq intbl                ; OriginalFirstThunk
    dd 0                    ; [UNUSED] TimeDateStamp
    dd dll_name             ; ForwarderChain
    dd iatbl                ; Name

intbl:                                                  ; Import Name Table
    dq symbol_GetStdHandle  ; [UNUSED] FirstThunk       ; Symbol
    dq symbol_WriteConsoleA                             ; nullptr             
    dq 0
itbl_size equ $-itbl

sect_size equ $-code
code_size equ $-code
file_size equ $-$$