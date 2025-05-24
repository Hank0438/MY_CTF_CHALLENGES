#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <windows.h>

#define MEMORY_SIZE     0x200
#define CODE_SIZE       0x400
#define STACK_SIZE      0x200
// #define KEY_SIZE        0x40
// #define BLOCK_SIZE      0x40

// opcode list
#define PUSH    0xAA
#define POP     0xBB
#define LOAD    0xCC
#define STORE   0xDD
#define XOR     0x20
#define OR      0x21
#define AND     0x22
#define ADD     0x23
#define SUB     0x24
#define CMP     0x25
#define JBZ     0x69
#define JFZ     0x96
#define JBNZ    0x78
#define JFNZ    0x87
#define PUTS    0xFF
#define EXIT    0xFE

#define INST1(oper, secret, src_id) \
    {PUSH, (secret)},       \
    {LOAD, (src_id)},       \
    {(oper), (src_id)},        

#define INST2(oper, src_id, dst_id) \
    {LOAD, (src_id)},       \
    {LOAD, (dst_id)},       \
    {(oper), (dst_id)},

typedef struct {
    uint8_t opcode;
    uint8_t operand;
} Instruction;

typedef struct {
    Instruction code[CODE_SIZE];
    uint8_t stack[STACK_SIZE];
    uint8_t memory[MEMORY_SIZE];
    uint8_t ip; // Instruction Pointer
    uint8_t sp; // Stack Pointer
    uint8_t zf; // Zero Flag
} VirtualMachine;

// typedef struct {
//     uint8_t left[BLOCK_SIZE/2];
//     uint8_t right[BLOCK_SIZE/2];
// } Block;

// typedef struct {
//     Block block;
//     uint8_t key[KEY_SIZE];
// } FeistelCipher;

void initializeVM(VirtualMachine *vm) {
    // Initialize memory, code, and stack
    for (int i = 0; i < MEMORY_SIZE; ++i) {
        vm->memory[i] = 0;
    }

    for (int i = 0; i < CODE_SIZE; ++i) {
        vm->code[i].opcode = 0;
        vm->code[i].operand = 0;
    }

    for (int i = 0; i < STACK_SIZE; ++i) {
        vm->stack[i] = 0;
    }

    vm->ip = 0;
    vm->sp = 0;
    vm->zf = 0;

}

void loadKey(VirtualMachine *vm, char* key, int size) {
    for (int i = 0; i < size; ++i) {
        vm->memory[i] = key[i];
    }
}

void loadProgram(VirtualMachine *vm, Instruction program[], int size) {
    // Load program into VM's code memory
    for (int i = 0; i < size; ++i) {
        vm->code[i] = program[i];
    }
}

void executeVM(VirtualMachine *vm) {
    while (vm->ip < CODE_SIZE) {
        Instruction currentInstruction = vm->code[vm->ip];

        switch (currentInstruction.opcode) {
            case PUSH:
                // Push the value at the operand index onto the stack
                vm->stack[vm->sp++] = currentInstruction.operand;
                break;
            case POP:
                // Pop the value from the stack to the operand index in memory
                vm->sp--;
                break;
            case LOAD:
                // LOAD
                vm->stack[vm->sp++] = vm->memory[currentInstruction.operand];
                break;
            case STORE:
                // STORE
                vm->memory[currentInstruction.operand] = vm->stack[--vm->sp];
                break;
            case XOR:
                // XOR
                vm->memory[currentInstruction.operand] = vm->stack[vm->sp-1] ^ vm->stack[vm->sp-2];
                vm->sp -= 2;
                break;
            case OR:
                // OR
                vm->memory[currentInstruction.operand] = vm->stack[vm->sp-1] | vm->stack[vm->sp-2];
                vm->sp -= 2;
                break;
            case AND:
                // AND
                vm->memory[currentInstruction.operand] = vm->stack[vm->sp-1] & vm->stack[vm->sp-2];
                vm->sp -= 2;
                break;
            case ADD:
                // ADD
                vm->memory[currentInstruction.operand] = vm->stack[vm->sp-1] + vm->stack[vm->sp-2];
                vm->sp -= 2;
                break;
            case SUB:
                // SUB
                vm->memory[currentInstruction.operand] = vm->stack[vm->sp-1] - vm->stack[vm->sp-2];
                vm->sp -= 2;
                break;
            case CMP:
                // CMP
                if (vm->stack[vm->sp-1] == vm->stack[vm->sp-2]) {
                    vm->zf = 1; // Set to true
                } else {
                    vm->zf = 0; // Set to false
                }
                vm->sp -= 2;
                break;
            case JBZ:
                // Jump backward by the operand if zero flag is 1
                if (vm->zf == 1) {
                    vm->ip -= currentInstruction.operand;
                }
                break;
            case JFZ:
                // Jump forward by the operand if zero flag is 1
                if (vm->zf == 1) {
                    vm->ip += currentInstruction.operand;
                }
                break;
            case JBNZ:
                // Jump backward by the operand if zero flag is 0
                if (vm->zf == 0) {
                    vm->ip -= currentInstruction.operand;
                }
                break;
            case JFNZ:
                // Jump forward by the operand if zero flag is 0
                if (vm->zf == 0) {
                    vm->ip += currentInstruction.operand;
                }
                break;
            case PUTS:
                // Output the value at the operand index in memory
                printf("%c", vm->memory[currentInstruction.operand]);
                break;
            case EXIT:
                // EXIT the VM
                TerminateProcess(GetCurrentProcess(), 1);
            default:
                // Invalid opcode, EXIT the VM
                printf("Invalid opcode. Exit VM.\n");
                return;
        }

        // Move to the next instruction
        ++vm->ip;
    }
}

int main(int argc, char *argv[]) {
    // flag: CGGC{ollvmvm_oolloolloovmvm_ollvmollvmvmollvmvm_vmollvm}
    // Challenge: Output the flag by executing the VM with the correct input
    Instruction challengeProgram[] = {
		INST1(XOR, 58, 2)
		INST2(SUB, 43, 22)
		INST2(SUB, 43, 34)
		INST1(XOR, 4, 14)
		INST2(XOR, 5, 22)
		INST2(ADD, 28, 33)
		INST1(ADD, 59, 22)
		INST1(XOR, 158, 0)
		INST1(XOR, 59, 1)
		INST1(XOR, 93, 17)
		INST2(ADD, 5, 40)
		INST1(ADD, 107, 24)
		INST2(SUB, 19, 25)
		INST1(XOR, 68, 3)
		INST2(XOR, 7, 46)
		INST1(ADD, 40, 33)
		INST1(XOR, 24, 50)
		INST2(SUB, 37, 55)
		INST1(XOR, 89, 1)
		INST2(XOR, 32, 10)
		INST2(SUB, 16, 9)
		INST2(ADD, 6, 35)
		INST1(ADD, 117, 25)
		INST1(ADD, 46, 34)
		INST2(ADD, 15, 6)
		INST2(ADD, 26, 7)
		INST1(ADD, 118, 2)
		INST2(XOR, 3, 15)
		INST1(ADD, 31, 50)
		INST1(ADD, 6, 50)
		INST2(XOR, 49, 25)
		INST2(XOR, 34, 40)
		INST1(ADD, 136, 16)
		INST1(XOR, 62, 55)
		INST2(ADD, 8, 39)
		INST1(SUB, 201, 2)
		INST2(XOR, 17, 13)
		INST2(SUB, 37, 49)
		INST1(ADD, 47, 44)
		INST1(XOR, 119, 32)
		INST2(ADD, 42, 25)
		INST1(SUB, 17, 21)
		INST1(ADD, 52, 44)
		INST2(ADD, 18, 46)
		INST1(XOR, 114, 41)
		INST1(XOR, 235, 24)
		INST2(XOR, 27, 16)
		INST1(XOR, 39, 52)
		INST2(SUB, 28, 54)
		INST1(XOR, 40, 19)
		INST1(XOR, 26, 52)
		INST1(XOR, 85, 46)
		INST2(XOR, 27, 44)
		INST2(ADD, 55, 21)
		INST1(ADD, 28, 30)
		INST1(ADD, 119, 11)
		INST2(XOR, 4, 51)
		INST2(XOR, 13, 41)
		INST1(XOR, 116, 46)
		INST2(ADD, 12, 23)
		{PUTS,0},
		{PUTS,1},
		{PUTS,2},
		{PUTS,3},
		{PUTS,4},
		{PUTS,5},
		{PUTS,6},
		{PUTS,7},
		{PUTS,8},
		{PUTS,9},
		{PUTS,10},
		{PUTS,11},
		{PUTS,12},
		{PUTS,13},
		{PUTS,14},
		{PUTS,15},
		{PUTS,16},
		{PUTS,17},
		{PUTS,18},
		{PUTS,19},
		{PUTS,20},
		{PUTS,21},
		{PUTS,22},
		{PUTS,23},
		{PUTS,24},
		{PUTS,25},
		{PUTS,26},
		{PUTS,27},
		{PUTS,28},
		{PUTS,29},
		{PUTS,30},
		{PUTS,31},
		{PUTS,32},
		{PUTS,33},
		{PUTS,34},
		{PUTS,35},
		{PUTS,36},
		{PUTS,37},
		{PUTS,38},
		{PUTS,39},
		{PUTS,40},
		{PUTS,41},
		{PUTS,42},
		{PUTS,43},
		{PUTS,44},
		{PUTS,45},
		{PUTS,46},
		{PUTS,47},
		{PUTS,48},
		{PUTS,49},
		{PUTS,50},
		{PUTS,51},
		{PUTS,52},
		{PUTS,53},
		{PUTS,54},
		{PUTS,55},
        {EXIT, 0},
    };

    VirtualMachine vm;
    initializeVM(&vm);
    printf("%llu\n", strlen(argv[1]));
    loadKey(&vm, argv[1], strlen(argv[1]));
    loadProgram(&vm, challengeProgram, sizeof(challengeProgram) / sizeof(challengeProgram[0]));

    printf("Executing VM...\n");
    executeVM(&vm);

    printf("\n");

    return 0;
}