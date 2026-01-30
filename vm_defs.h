#ifndef VM_DEFS_H
#define VM_DEFS_H

// Registers
enum {
    AX, // General purpose register
    BX, // General purpose register
    CX, // General purpose register
    DX, // General purpose register
    SP, // Stack pointer
    IP, // Instruction pointer
    FLAGS, // Flags register (e.g., Zero, Equal)
    NUM_REGISTERS
};

// Instruction set
enum {
    HLT,         // 0 Halt
    MOV,         // 1 Move value to register (reg, val)
    ADD,         // 2 Add value to register (reg, val)
    SUB,         // 3 Subtract value from register (reg, val)
    MOV_REG_REG, // 4 Move register to register (reg1, reg2)
    ADD_REG_REG, // 5 Add register to register (reg1, reg2)
    SUB_REG_REG, // 6 Subtract register from register (reg1, reg2)
    JMP,         // 7 Unconditional jump (address)
    CMP,         // 8 Compare two values, set FLAGS (val1, val2)
    JE,          // 9 Jump if Equal (address)
    JNE,         // 10 Jump if Not Equal (address)
};

#endif
