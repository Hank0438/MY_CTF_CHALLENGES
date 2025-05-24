use std::str;
use std::io::{self, Read, Write};
use std::process;
use rand::{seq::SliceRandom, Rng};

struct RC4 {
    state: [u8; 256],
    i: u8,
    j: u8,
}

impl RC4 {
    fn new(key: &[u8]) -> RC4 {
        assert!(key.len() >= 1 && key.len() <= 256);
        let mut rc4 = RC4 { i: 0, j: 0, state: [0; 256] };
        for (i, x) in rc4.state.iter_mut().enumerate() {
            *x = i as u8;
        }
        let mut j: u8 = 0;
        for i in 0..256 {
            j = j.wrapping_add(rc4.state[i]).wrapping_add(key[i % key.len()]);
            rc4.state.swap(i, j as usize);
        }
        rc4
    }
        
    fn generate_byte(&mut self) -> u8 {
        self.i = self.i.wrapping_add(1);
        self.j = self.j.wrapping_add(self.state[self.i as usize]);
        self.state.swap(self.i as usize, self.j as usize);
        let k = self.state[(self.state[self.i as usize].wrapping_add(self.state[self.j as usize])) as usize];
        k
    }

    fn encrypt(&mut self, data: &[u8]) -> Vec<u8> {
        let mut result = Vec::with_capacity(data.len());
        for &byte in data {
            result.push(byte ^ self.generate_byte());
        }
        // println!("[DBG] Ciphertext: {:?}", result);
        result
    }
}

// fn test_RC4() {
//     let key = b"SecretKey";
//     let plaintext = b"Hello, RC4!";
    
//     let mut rc4 = RC4::new(key);
//     let ciphertext = rc4.encrypt(plaintext);

//     println!("Plaintext: {:?}", plaintext);
//     println!("Ciphertext: {:?}", ciphertext);
// }

struct VM {
    memory: Vec<u8>,
    ip: usize,  // Instruction Pointer
    stack: Vec<u8>,
}

impl VM {
    fn new() -> Self {
        Self {
            memory: Vec::new(),
            ip: 0,
            stack: Vec::new(),
        }
    }

    fn load_program(&mut self, program: Vec<u8>) {
        self.memory = program;
        self.ip = 0;
    }

    fn load_flag(&mut self, flag: Vec<u8>) {
        self.stack = flag;
    }

    fn shuffle_flag(&mut self, flag: Vec<u8>) -> Vec<u8> {
        // Get the length of the vector
        let len = flag.len();
    
        // Create a vector of indices from 0 to len - 1
        let indices: Vec<usize> = (0..len).collect();
    
        // Specify the order in which you want to shuffle the indices
        // python3 -c "import random; a=[i for i in range(40)]; random.shuffle(a); print(a)"
        let order = vec![2, 36, 9, 39, 16, 35, 14, 12, 11, 37, 1, 26, 29, 27, 32, 31, 34, 23, 30, 0, 4, 19, 15, 6, 33, 17, 24, 22, 28, 38, 25, 20, 5, 10, 7, 18, 13, 21, 3, 8];
    
        // Shuffle the indices based on the given order
        // let shuffled_indices: Vec<usize> = order
        //     .choose_multiple(&mut rand::thread_rng(), len)
        //     .cloned()
        //     .collect();
    
        // Create a new vector by rearranging the elements using shuffled indices
        let shuffled_flag: Vec<u8> = order.iter().map(|&i| flag[i]).collect();
        
        // Print the original and shuffled vectors
        // println!("[DBG] Original data: {:?}", flag);
        // println!("[DBG] Shuffled data: {:?}", shuffled_flag);
        shuffled_flag
    }

    fn execute(&mut self) {

        while (self.ip < self.memory.len()) && (self.stack.len() > 0) {
            let current_inst = self.memory[self.ip];
            println!("[VM] {}: the current instruction: {}", self.ip, current_inst);
            match current_inst {
                0x01 => self.instruction_push(),
                0x02 => self.instruction_pop(),
                0x03 => self.instruction_add(),
                0x04 => self.instruction_sub(),
                0x05 => self.instruction_mul(),
                0x06 => self.instruction_div(),
                0x07 => self.instruction_mod(),
                0x08 => self.instruction_and(),
                0x09 => self.instruction_or(),
                0x0A => self.instruction_xor(),
                0x0B => self.instruction_not(),
                0x0C => self.instruction_rol(),
                0x0D => self.instruction_ror(),
                0x0E => self.instruction_dup(),
                0x0F => self.instruction_swap(),
                0x10 => self.instruction_rot(),
                0x11 => self.instruction_over(),
                0x12 => self.instruction_halt(),
                0x13 => self.instruction_puts_all(),
                0x14 => self.instruction_branch_if_zero(),
                0x15 => self.instruction_branch_if_positive(),
                0x16 => self.instruction_branch_if_negative(),
                0x17 => self.instruction_nop(),
                _ => {
                    println!("[VM] Unknown instruction encountered.");
                    return;
                }
            }
            self.ip += 1;
        }
        if (self.stack.len() > 0){
            self.instruction_puts_all();
        }
        println!("[VM] Done.");
    }

    fn instruction_push(&mut self) {
        self.ip += 1;
        let value = self.memory[self.ip];
        self.stack.push(value);
    }

    fn instruction_pop(&mut self) {
        if let Some(value) = self.stack.pop() {
            println!("[OUTPUT] {} ", value);
        } else {
            // println!("[INST] Stack is empty.");
            self.exit();
        }
    }

    fn instruction_add(&mut self) {
        if let (Some(a), Some(b)) = (self.stack.pop(), self.stack.pop()) {
            let result = a.wrapping_add(b) & 0xff;
            self.stack.push(result);
        } else {
            // println!("[INST] Not enough values on the stack for addition.");
            self.exit();
        }
    }

    fn instruction_sub(&mut self) {
        if let (Some(a), Some(b)) = (self.stack.pop(), self.stack.pop()) {
            let result = a.wrapping_sub(b) & 0xff;
            self.stack.push(result);
        } else {
            // println!("[INST] Not enough values on the stack for subtraction.");
            self.exit();
        }
    }

    fn instruction_mul(&mut self) {
        if let (Some(a), Some(b)) = (self.stack.pop(), self.stack.pop()) {
            let result = a.wrapping_mul(b);
            self.stack.push(result);
        } else {
            // println!("[INST] Not enough values on the stack for multiplication.");
            self.exit();
        }
    }

    fn instruction_div(&mut self) {
        if let (Some(a), Some(b)) = (self.stack.pop(), self.stack.pop()) {
            if b != 0 {
                let result = a.wrapping_div(b);
                self.stack.push(result);
            } else {
                // println!("[INST] Division by zero error.");
                self.exit();
            }
        } else {
            // println!("[INST] Not enough values on the stack for division.");
            self.exit();
        }
    }

    fn instruction_mod(&mut self) {
        if let (Some(a), Some(b)) = (self.stack.pop(), self.stack.pop()) {
            if b != 0 {
                let result = a.wrapping_rem(b);
                self.stack.push(result);
            } else {
                // println!("[INST] Modulo by zero error.");
            }
        } else {
            // println!("[INST] Not enough values on the stack for modulo operation.");
            self.exit();
        }
    }

    fn instruction_and(&mut self) {
        if let (Some(a), Some(b)) = (self.stack.pop(), self.stack.pop()) {
            let result = a & b;
            self.stack.push(result);
        } else {
            // println!("[INST] Not enough values on the stack for bitwise AND operation.");
            self.exit();
        }
    }

    fn instruction_or(&mut self) {
        if let (Some(a), Some(b)) = (self.stack.pop(), self.stack.pop()) {
            let result = a | b;
            self.stack.push(result);
        } else {
            // println!("[INST] Not enough values on the stack for bitwise OR operation.");
            self.exit();
        }
    }

    fn instruction_xor(&mut self) {
        if let (Some(a), Some(b)) = (self.stack.pop(), self.stack.pop()) {
            let result = a ^ b;
            self.stack.push(result);
        } else {
            // println!("[INST] Not enough values on the stack for bitwise XOR operation.");
            self.exit();
        }
    }

    fn instruction_not(&mut self) {
        if let Some(a) = self.stack.pop() {
            let result = !a;
            // println!("[DBG] {} {} ", a, result);
            self.stack.push(result);
        } else {
            // println!("[INST] Not enough values on the stack for bitwise NOT operation.");
            self.exit();
        }
    }

    fn instruction_rol(&mut self) {
        if let (Some(b), Some(a)) = (self.stack.pop(), self.stack.pop()) {
            let result = (a.wrapping_shl(u32::from(b)) | a.wrapping_shr(u32::from(8-b))) & 0xff;
            self.stack.push(result);
        } else {
            // println!("[INST] Not enough values on the stack for left shift operation.");
            self.exit();
        }
    }

    fn instruction_ror(&mut self) {
        if let (Some(b), Some(a)) = (self.stack.pop(), self.stack.pop()) {
            let result = (a.wrapping_shr(u32::from(b)) | a.wrapping_shl(u32::from(8-b))) & 0xff;
            self.stack.push(result);
        } else {
            // println!("[INST] Not enough values on the stack for right shift operation.");
            self.exit();
        }
    }

    fn instruction_dup(&mut self) {
        if let Some(a) = self.stack.last().cloned() {
            // println!("[DBG] DUP {} ", a);
            self.stack.push(a);
        } else {
            // println!("[INST] Stack is empty. Cannot duplicate top value.");
            self.exit();
        }
    }

    fn instruction_swap(&mut self) {
        if let (Some(a), Some(b)) = (self.stack.pop(), self.stack.pop()) {
            self.stack.push(a);
            self.stack.push(b);
        } else {
            // println!("[INST] Not enough values on the stack for swap operation.");
            self.exit();
        }
    }

    fn instruction_rot(&mut self) {
        if self.stack.len() >= 3 {
            let a = self.stack.pop().unwrap();
            let b = self.stack.pop().unwrap();
            let c = self.stack.pop().unwrap();
            self.stack.push(b);
            self.stack.push(a);
            self.stack.push(c);
        } else {
            // println!("[INST] Not enough values on the stack for rotation operation.");
            self.exit();
        }
    }

    fn instruction_over(&mut self) {
        if self.stack.len() >= 2 {
            let a = self.stack[self.stack.len() - 2];
            self.stack.push(a);
        } else {
            // println!("[INST] Not enough values on the stack for over operation.");
            self.exit();
        }
    }

    fn instruction_halt(&mut self) {
        // println!("[INST] Halt instruction encountered. Program execution halted.");
        process::exit(i32::from(self.memory[self.ip]));
    }

    fn instruction_puts_all(&mut self) {
        while self.stack.len() > 0 {
            if let Some(value) = self.stack.pop() {
                println!("[OUTPUT] {} ", value);
                // if let Ok(s) = str::from_utf8(&[value]) {
                //     println!("{}", s);
                // }
            } else {
                // println!("[INST] Stack is empty.");
            }
        }     
    }

    fn instruction_branch_if_zero(&mut self) {
        if let Some(condition) = self.stack.pop() {
            self.ip += 1;
            if condition == 0 {
                // Jump to the specified address (read next byte as address)
                let jump_address = self.memory[self.ip] as usize;
                self.ip += jump_address;
            }
        } else {
            // println!("[INST] Not enough values on the stack for conditional branch.");
            self.exit();
        }
    }

    fn instruction_branch_if_positive(&mut self) {
        if let Some(condition) = self.stack.pop() {
            self.ip += 1;
            if condition > 0 {
                // Jump to the specified address (read next byte as address)
                let jump_address = self.memory[self.ip] as usize;
                self.ip += jump_address;
            }
        } else {
            // println!("[INST] Not enough values on the stack for conditional branch.");
            self.exit();
        }
    }

    fn instruction_branch_if_negative(&mut self) {
        if let Some(condition) = self.stack.pop() {
            self.ip += 1;
            if condition < 0 {
                // Jump to the specified address (read next byte as address)
                let jump_address = self.memory[self.ip] as usize;
                self.ip += jump_address;
            }
        } else {
            // println!("[INST] Not enough values on the stack for conditional branch.");
            self.exit();
        }
    }

    fn instruction_nop(&mut self) {
        let a = 1;
    }

    fn exit(&mut self) {
        self.instruction_puts_all();
        self.instruction_halt();
    }
}


fn main() {
    let key = b"SecretKey";
    let mut rc4 = RC4::new(key);
    let mut vm = VM::new();

    // Create a buffer to store the input
    let mut input_buffer = String::new();

    // Prompt the user to enter input
    print!("[+] Enter encrypted flag: ");
    io::stdout().flush().unwrap();

    // Read input from stdin
    io::stdin().read_line(&mut input_buffer).expect("[x] Failed to read line");

    // Trim any leading or trailing whitespace
    let trimmed_input = input_buffer.trim();

    // Check if the input has at least 48 characters
    // println!("[DBG] Flag: {}", trimmed_input);
    if !(trimmed_input.len() == 40) {
        println!("[x] Input should have 40 characters.");
        process::exit(0);
    }
    let flag = trimmed_input.as_bytes().to_vec();
    

    /* VM bytecode program */
    // let bytecode_program = vec![
    //     0x1,0x42,   // push 0x42
    //     0x1,0x43,   // push 0x42
    //     0x4,        // sub
    //     0x2,        // pop
    //     0x13,       // puts
    // ];

	let bytecode_program = vec![
		0x1, 0x7, 	// PUSH 0x7
		0x11, 	// OVER
		0x1, 0x2, 	// PUSH 0x2
		0x3, 	// ADD
		0x1, 0x80, 	// PUSH 0x80
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x5, 	// PUSH 0x5
		0xc, 	// ROL
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x2, 	// PUSH 0x2
		0xa, 	// XOR
		0x1, 0x6, 	// PUSH 0x6
		0xf, 	// SWAP
		0x1, 0x6, 	// PUSH 0x6
		0xd, 	// ROR
		0x1, 0x4, 	// PUSH 0x4
		0xf, 	// SWAP
		0x1, 0x6, 	// PUSH 0x6
		0xa, 	// XOR
		0x1, 0x6, 	// PUSH 0x6
		0xa, 	// XOR
		0x1, 0x2, 	// PUSH 0x2
		0x3, 	// ADD
		0x2, 	// POP
		0x1, 0x5, 	// PUSH 0x5
		0xa, 	// XOR
		0x1, 0x6, 	// PUSH 0x6
		0x10, 	// ROT
		0x2, 	// POP
		0x1, 0xe2, 	// PUSH 0xe2
		0x1, 0x1, 	// PUSH 0x1
		0x10, 	// ROT
		0x2, 	// POP
		0x1, 0x1, 	// PUSH 0x1
		0xa, 	// XOR
		0x1, 0x1, 	// PUSH 0x1
		0xa, 	// XOR
		0x2, 	// POP
		0x1, 0x2, 	// PUSH 0x2
		0x11, 	// OVER
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x2, 	// PUSH 0x2
		0xc, 	// ROL
		0x2, 	// POP
		0x1, 0x4, 	// PUSH 0x4
		0xd, 	// ROR
		0x1, 0x3, 	// PUSH 0x3
		0xf, 	// SWAP
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x1, 	// PUSH 0x1
		0xf, 	// SWAP
		0x1, 0x4, 	// PUSH 0x4
		0xe, 	// DUP
		0x2, 	// POP
		0x1, 0x0, 	// PUSH 0x0
		0x11, 	// OVER
		0x1, 0x40, 	// PUSH 0x40
		0x1, 0x7, 	// PUSH 0x7
		0xd, 	// ROR
		0x2, 	// POP
		0x1, 0xda, 	// PUSH 0xda
		0x1, 0x3, 	// PUSH 0x3
		0x11, 	// OVER
		0x1, 0x3, 	// PUSH 0x3
		0xd, 	// ROR
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x4, 	// PUSH 0x4
		0xa, 	// XOR
		0x1, 0x7, 	// PUSH 0x7
		0x10, 	// ROT
		0x1, 0x5, 	// PUSH 0x5
		0xb, 	// NOT
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x2, 	// PUSH 0x2
		0x10, 	// ROT
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x6, 	// PUSH 0x6
		0xb, 	// NOT
		0x1, 0x14, 	// PUSH 0x14
		0x1, 0x7, 	// PUSH 0x7
		0x4, 	// SUB
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x6, 	// PUSH 0x6
		0xf, 	// SWAP
		0x1, 0xd4, 	// PUSH 0xd4
		0x2, 	// POP
		0x1, 0x3, 	// PUSH 0x3
		0xb, 	// NOT
		0x1, 0x0, 	// PUSH 0x0
		0xb, 	// NOT
		0x2, 	// POP
		0x1, 0x4, 	// PUSH 0x4
		0x4, 	// SUB
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x4, 	// PUSH 0x4
		0xe, 	// DUP
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x1, 	// PUSH 0x1
		0x11, 	// OVER
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x7, 	// PUSH 0x7
		0xf, 	// SWAP
		0x1, 0x4, 	// PUSH 0x4
		0xb, 	// NOT
		0x2, 	// POP
		0x1, 0x1, 	// PUSH 0x1
		0xd, 	// ROR
		0x2, 	// POP
		0x1, 0x3, 	// PUSH 0x3
		0xd, 	// ROR
		0x2, 	// POP
		0x1, 0x12, 	// PUSH 0x12
		0x1, 0x7, 	// PUSH 0x7
		0xb, 	// NOT
		0x2, 	// POP
		0x1, 0x1, 	// PUSH 0x1
		0xf, 	// SWAP
		0x1, 0x4, 	// PUSH 0x4
		0xd, 	// ROR
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0xca, 	// PUSH 0xca
		0x15, 0x2, 	// JNE 0x2
		0x3, 	// ADD
		0x2, 	// POP
		0x1, 0x48, 	// PUSH 0x48
		0x14, 0x3, 	// JE 0x3
		0x11, 	// OVER
		0xe, 	// DUP
		0x11, 	// OVER
		0x1, 0x2, 	// PUSH 0x2
		0xb, 	// NOT
		0x2, 	// POP
		0x1, 0x6, 	// PUSH 0x6
		0xd, 	// ROR
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x7, 	// PUSH 0x7
		0xe, 	// DUP
		0x1, 0x0, 	// PUSH 0x0
		0x10, 	// ROT
		0x1, 0x7, 	// PUSH 0x7
		0xd, 	// ROR
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x0, 	// PUSH 0x0
		0xb, 	// NOT
		0x1, 0xf6, 	// PUSH 0xf6
		0x1, 0x0, 	// PUSH 0x0
		0xf, 	// SWAP
		0x1, 0x3, 	// PUSH 0x3
		0xa, 	// XOR
		0x1, 0x4, 	// PUSH 0x4
		0xb, 	// NOT
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x3, 	// PUSH 0x3
		0xe, 	// DUP
		0x2, 	// POP
		0x1, 0x2, 	// PUSH 0x2
		0xd, 	// ROR
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x3, 	// PUSH 0x3
		0x10, 	// ROT
		0x1, 0x7, 	// PUSH 0x7
		0x10, 	// ROT
		0x1, 0x4, 	// PUSH 0x4
		0xb, 	// NOT
		0x2, 	// POP
		0x1, 0x0, 	// PUSH 0x0
		0x10, 	// ROT
		0x1, 0x1, 	// PUSH 0x1
		0xd, 	// ROR
		0x2, 	// POP
		0x1, 0x4, 	// PUSH 0x4
		0x11, 	// OVER
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x1, 	// PUSH 0x1
		0xb, 	// NOT
		0x2, 	// POP
		0x1, 0xca, 	// PUSH 0xca
		0x15, 0x6, 	// JNE 0x6
		0x11, 	// OVER
		0x11, 	// OVER
		0xe, 	// DUP
		0x4, 	// SUB
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x3, 	// PUSH 0x3
		0xa, 	// XOR
		0x2, 	// POP
		0x1, 0x2, 	// PUSH 0x2
		0xb, 	// NOT
		0x1, 0x6, 	// PUSH 0x6
		0x11, 	// OVER
		0x1, 0x4, 	// PUSH 0x4
		0xa, 	// XOR
		0x2, 	// POP
		0x1, 0xca, 	// PUSH 0xca
		0x15, 0x0, 	// JNE 0x0
		0x2, 	// POP
		0x1, 0x6, 	// PUSH 0x6
		0xd, 	// ROR
		0x2, 	// POP
		0x1, 0xd6, 	// PUSH 0xd6
		0x14, 0x2, 	// JE 0x2
		0x2, 	// POP
		0xf, 	// SWAP
		0x1, 0x3, 	// PUSH 0x3
		0x10, 	// ROT
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x5, 	// PUSH 0x5
		0xf, 	// SWAP
		0x1, 0x5, 	// PUSH 0x5
		0xb, 	// NOT
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x2, 	// PUSH 0x2
		0x11, 	// OVER
		0x1, 0x5, 	// PUSH 0x5
		0xe, 	// DUP
		0x2, 	// POP
		0x1, 0x4, 	// PUSH 0x4
		0xc, 	// ROL
		0x2, 	// POP
		0x1, 0x0, 	// PUSH 0x0
		0xe, 	// DUP
		0x1, 0x5, 	// PUSH 0x5
		0xd, 	// ROR
		0x1, 0x5, 	// PUSH 0x5
		0x10, 	// ROT
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x2, 	// PUSH 0x2
		0xe, 	// DUP
		0x1, 0x7, 	// PUSH 0x7
		0xd, 	// ROR
		0x1, 0x6, 	// PUSH 0x6
		0xc, 	// ROL
		0x1, 0x2, 	// PUSH 0x2
		0xb, 	// NOT
		0x2, 	// POP
		0x1, 0x4, 	// PUSH 0x4
		0xa, 	// XOR
		0x2, 	// POP
		0x1, 0x2, 	// PUSH 0x2
		0xc, 	// ROL
		0x1, 0xbc, 	// PUSH 0xbc
		0x1, 0x1, 	// PUSH 0x1
		0xf, 	// SWAP
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0xbe, 	// PUSH 0xbe
		0x1, 0x6, 	// PUSH 0x6
		0xc, 	// ROL
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x1, 	// PUSH 0x1
		0xa, 	// XOR
		0x1, 0x4, 	// PUSH 0x4
		0x11, 	// OVER
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x4, 	// PUSH 0x4
		0xf, 	// SWAP
		0x1, 0x5, 	// PUSH 0x5
		0x11, 	// OVER
		0x1, 0x7a, 	// PUSH 0x7a
		0x2, 	// POP
		0x1, 0x4, 	// PUSH 0x4
		0xb, 	// NOT
		0x1, 0x2, 	// PUSH 0x2
		0x10, 	// ROT
		0x1, 0x7, 	// PUSH 0x7
		0xf, 	// SWAP
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x9c, 	// PUSH 0x9c
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x3, 	// PUSH 0x3
		0xf, 	// SWAP
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x3, 	// PUSH 0x3
		0xc, 	// ROL
		0x2, 	// POP
		0x1, 0x3, 	// PUSH 0x3
		0xe, 	// DUP
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x7, 	// PUSH 0x7
		0xa, 	// XOR
		0x1, 0x3, 	// PUSH 0x3
		0x11, 	// OVER
		0x2, 	// POP
		0x1, 0xa0, 	// PUSH 0xa0
		0x1, 0x7, 	// PUSH 0x7
		0xd, 	// ROR
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x7, 	// PUSH 0x7
		0xd, 	// ROR
		0x2, 	// POP
		0x1, 0x7, 	// PUSH 0x7
		0x10, 	// ROT
		0x1, 0x0, 	// PUSH 0x0
		0xf, 	// SWAP
		0x2, 	// POP
		0x1, 0x2, 	// PUSH 0x2
		0x11, 	// OVER
		0x1, 0x6, 	// PUSH 0x6
		0x10, 	// ROT
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x0, 	// PUSH 0x0
		0xa, 	// XOR
		0x2, 	// POP
		0x1, 0x44, 	// PUSH 0x44
		0x15, 0x3, 	// JNE 0x3
		0xa, 	// XOR
		0xb, 	// NOT
		0xe, 	// DUP
		0x1, 0x6, 	// PUSH 0x6
		0xf, 	// SWAP
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x7e, 	// PUSH 0x7e
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x4, 	// PUSH 0x4
		0x10, 	// ROT
		0x1, 0x2, 	// PUSH 0x2
		0xb, 	// NOT
		0x2, 	// POP
		0x1, 0x0, 	// PUSH 0x0
		0x10, 	// ROT
		0x1, 0x4, 	// PUSH 0x4
		0x10, 	// ROT
		0x2, 	// POP
		0x1, 0x2, 	// PUSH 0x2
		0xd, 	// ROR
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x3, 	// PUSH 0x3
		0xb, 	// NOT
		0x1, 0x5, 	// PUSH 0x5
		0x10, 	// ROT
		0x1, 0xfc, 	// PUSH 0xfc
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x4, 	// PUSH 0x4
		0xe, 	// DUP
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
		0x1, 0x7, 	// PUSH 0x7
		0xb, 	// NOT
		0x1, 0x4, 	// PUSH 0x4
		0xd, 	// ROR
		0x2, 	// POP
		0x1, 0x6, 	// PUSH 0x6
		0xb, 	// NOT
		0x1, 0x0, 	// PUSH 0x0
		0xc, 	// ROL
		0x2, 	// POP
		0x1, 0x3, 	// PUSH 0x3
		0xc, 	// ROL
		0x1, 0x0, 	// PUSH 0x0
		0xd, 	// ROR
		0x2, 	// POP
		0x2, 	// POP
		0x2, 	// POP
	];


    let encrypted_flag = rc4.encrypt(&flag);
    let shuffled_flag = vm.shuffle_flag(encrypted_flag);

    vm.load_program(bytecode_program);
    vm.load_flag(shuffled_flag);
    vm.execute();
}
