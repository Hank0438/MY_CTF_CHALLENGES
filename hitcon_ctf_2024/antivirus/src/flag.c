
VIRUSNAME_PREFIX("PRINT_FLAG")
VIRUSNAMES("Real", "Fake")
SIGNATURES_DECL_BEGIN
DECLARE_SIGNATURE(MZ)
SIGNATURES_DECL_END

TARGET(0) // ANY

SIGNATURES_DEF_BEGIN
DEFINE_SIGNATURE(MZ, "0:4d5a")
SIGNATURES_END

#define CLI_ROL(a,b) a = (a << b) | (a >> (8-b))
#define CLI_ROR(a,b) a = (a >> b) | (a << (8-b))
#define cli_dbgmsg(...)

static inline  __attribute__((always_inline)) int emulator(char* decryptor_inst, char* code, unsigned int len) {
    unsigned char al;
    unsigned char bl;
    unsigned char cl = len & 0x7;
    unsigned int j,i;

    for(i=0;i<len;i++) {
        al = i & 0xff;
        bl = code[i];
        
        for(j=0;j<0x20;j++) {
            switch(decryptor_inst[j]) {
                case '\xEB':	/* JMP short */
                    j++;
                    if(bl<0x80)     /* Conditional branch */
                    {
                        break;
                    }
                    j = j + decryptor_inst[j];
                    break;

                case '\xFE':	/* DEC  AL */
                    al--;
                    break;

                case '\x2A':	/* SUB AL,CL */
                    al = al - cl;
                    break;

                case '\x02':	/* ADD AL,CL */
                    al = al + cl;
                    break;

                case '\x32':	/* XOR AL,CL */
                    al = al ^ cl;
                    break;
                
                case '\x04':	/* ADD AL,num */
                    j++;
                    al = al + decryptor_inst[j];
                    break;
                
                case '\x34':	/* XOR AL,num */
                    j++;
                    al = al ^ decryptor_inst[j];
                    break;

                case '\x2C':	/* SUB AL,num */
                    j++;
                    al = al - decryptor_inst[j];
                    break;

                case '\xC0':
                    j++;
                    if(decryptor_inst[j]=='\xC0') /* ROL AL,2 */
                    {
                        CLI_ROL(al,2);
                    }
                    else			/* ROR AL,2 */
                    {
                        CLI_ROR(al,2);
                    }
                    break;

                case '\xD2':
                    j++;
                    if(decryptor_inst[j]=='\xC8') /* ROR AL,6 */
                    {
                        CLI_ROR(al,6);
                    }
                    else			/* ROL AL,6 */
                    {
                        CLI_ROL(al,6);
                    }
                    break;

                case '\x90':
                case '\xf8':
                case '\xf9':
                    break;

                default:
                    return 1;
            }
        }
        
        al &= 0xff;
        code[i] = (al ^ bl)&0xff;
    }
    return 0;

}

int encrypt(char* filedata, uint32_t filesize) {
    unsigned i;
    char decryptor_inst[] = {
        0x2c, 0x5, 0xd2, 0xf8, 0x4, 0x3, 0x34, 0x6, 
        0xfe, 0xeb, 0x6, 0xc0, 0xc0, 0xf9, 0x2, 0x90, 
        0xc0, 0xf9, 0x2a, 0xd2, 0xc8, 0xf8, 0x32, 0x90, 
        0x90, 0xf9, 0x90, 0xf8, 0xf8, 0xf9, 0x90, 0xf8    
    };
    
    emulator(decryptor_inst, filedata, filesize);
    
    return 1;
}

bool logical_trigger(void) {
  return matches(Signatures.MZ);
}

int entrypoint() {
    int filesize = seek(0, SEEK_END);
    char filedata[0x400] = {0};
    int i;
    
    char encrypted_executable[] = {
        0x45,0x5f,0x6,0x7,0x54,0x45,0x1,0x2,
        0x67,0x75,0xfd,0xfd,0xfe,0xfb,0xf8,0xf9,
        0xfa,0xf6,0xf7,0xf4,0xf5,0xf3,0xf0,0xf1,
        0x76,0x2c,0xf,0x2e,0x24,0x2a,0x29,0x2a,
        0xad,0x25,0x25,0x26,0x27,0x20,0x21,0x22,
        0x23,0x1c,0x1d,0x1e,0x39,0x19,0x19,0x1a,
        0x11,0x14,0x15,0x16,0x17,0x10,0x11,0x52,
        0x12,0x4c,0x4d,0x4e,0x4b,0x48,0x49,0x4a,
        0x4f,0x44,0x45,0x46,0x41,0x40,0x41,0x42,
        0x43,0x3c,0x3d,0x3e,0x39,0x38,0x39,0x3a,
        0x3b,0x34,0x35,0x36,0xa7,0x31,0x31,0x32,
        0xde,0x6c,0x6d,0x6e,0x6f,0x68,0x69,0x6a,
        0x68,0x64,0x5,0xdb,0x67,0x60,0x71,0x62,
        0x63,0x5c,0x5d,0x5e,0x5f,0x48,0x59,0x5a,
        0x5b,0x54,0x55,0x56,0x57,0x50,0x41,0x52,
        0x53,0x8c,0x8d,0x8e,0x8f,0x98,0x89,0x8a,
        0x8b,0x84,0x85,0x86,0x87,0x80,0x81,0x82,
        0x81,0x7c,0x7d,0x7e,0x7f,0x78,0x79,0x7a,
        0x7b,0x74,0x75,0x76,0x17,0x71,0x71,0x72,
        0x5f,0xac,0xad,0xae,0x81,0xa8,0xa9,0xaa,
        0xab,0xa4,0xa5,0xa6,0x19,0xa1,0xa1,0xa2,
        0xb5,0x9d,0x9d,0x9e,0x11,0x99,0x99,0x9a,
        0x8d,0x95,0x95,0x96,0x2b,0x35,0x29,0x3d,
        0x30,0x66,0x72,0x79,0x6e,0x67,0x77,0x63,
        0x73,0x2f,0x4,0x1c,0x3,0x6d,0x6e,0x65,
        0x6a,0xa,0x26,0x13,0x15,0x2b,0x16,0x1a,
        0x16,0xd,0x0,0x14,0x3c,0x1e,0x4,0xa,
        0x1a,0x4b,0x46,0x4e,0x4e,0x7b,0x56,0x4f,
        0x40,0x7e,0x70,0x66,0x66,0x5e,0x48,0x53,
        0xe3,0x44,0xdf,0x99,0xba,0xac,0x8a,0xae,
        0xbf,0x9c,0xb4,0xb8,0xb3,0xbc,0xb4,0xd2,
        0x42,0xf,0x59,0x7d,0x65,0x7d,0x6f,0x48,
        0x67,0x6b,0x75,0x68,0x68,0x65,0x40,0x2,
        0x48,0xba,0xae,0xb3,0xbb,0xb7,0xcb,0xcb,
        0xd4,0x92,0x9b,0x98,0xf5,0xf3,0x1d,0xf1,
        0xf2,0x2c,0x2d,0x2e,0x2f,0x28,0xd5,0x2a,
        0x2b,0x24,0x25,0x26,0x27,0x20,0x9c,0xd3,
        0xd8,0xef,0xee,0xed,0xa,0xf9,0xe2,0xe1,
        0xe0,0x51,0x24,0xc3,0x52,0x99,0x50,0x27,
        0x5b,0xcd,0x58,0x3b,0xbc,0xb3,0xb2,0x2,
        0x7a,0xf1,0xb9,0x72,0x4d,0x8c,0x9,0xb9,
        0x86,0x74,0xb2,0xcb,0xa,0x4d,0xce,0x72,
        0xb2,0x21,0x6b,0xd5,0xd4,0xcb,0x79,0xbf,
        0xf6,0x9f,0x78,0xdd,0x9c,0x93,0x92,0xad,
        0x1f,0x65,0x65,0x66,0x67,0x60,0x61,0x62,
        0x63,0x5c,0x5d,0x5e,0x57,0x59,0x59,0x5a,
        0x4d,0x55,0x55,0x56,0xa2,0x50,0x51,0x52,
        0x53,0x8c,0x8d,0x8e,0x7b,0x88,0x89,0x8a,
        0x8b,0x84,0x85,0x86,0x87,0x80,0x81,0x82,
        0x83,0x7c,0x7d,0x7e

    };

    if (filesize != 396) {
        foundVirus("Fake");
        return 1;
    }

    seek(0, SEEK_SET);
    read((uint8_t*)filedata, filesize);
    
    // Run decryptor
    int res = encrypt(filedata, filesize);
    
    for (i=0;i<filesize;i++) {
        if (encrypted_executable[i] != filedata[i])
            return 0; 
    }

    foundVirus("Real");
    return 1;
}
