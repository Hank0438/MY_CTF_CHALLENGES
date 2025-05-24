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
PUT_SOME_VM_CODE_HERE
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