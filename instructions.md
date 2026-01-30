# Instruction Set

This document describes the instruction set for the virtual machine.

| Instruction   | Opcode | Arguments              | Description                                                    |
|---------------|--------|------------------------|----------------------------------------------------------------|
| `HLT`         | 0      | None                   | Halts the execution of the program.                            |
| `MOV`         | 1      | `reg`, `val`           | Moves an immediate value into a register.                      |
| `ADD`         | 2      | `reg`, `val`           | Adds an immediate value to a register.                         |
| `SUB`         | 3      | `reg`, `val`           | Subtracts an immediate value from a register.                  |
| `MOV_REG_REG` | 4      | `reg1`, `reg2`         | Moves the value from `reg2` into `reg1`.                       |
| `ADD_REG_REG` | 5      | `reg1`, `reg2`         | Adds the value from `reg2` to `reg1`.                          |
| `SUB_REG_REG` | 6      | `reg1`, `reg2`         | Subtracts the value from `reg2` from `reg1`.                   |
| `JMP`         | 7      | `address`              | Unconditionally jumps to the specified memory `address`.       |
| `CMP`         | 8      | `arg1`, `arg2`         | Compares `arg1` and `arg2`. Sets `FLAGS` register if equal.    |
| `JE`          | 9      | `address`              | Jumps to `address` if the `FLAGS` register indicates equality. |
| `JNE`         | 10     | `address`              | Jumps to `address` if the `FLAGS` register indicates inequality.|

## Instruction Format

*   **HLT**: `HLT`
*   **MOV**: `MOV <register>, <value>`
*   **ADD**: `ADD <register>, <value>`
*   **SUB**: `SUB <register>, <value>`
*   **MOV_REG_REG**: `MOV <register1>, <register2>`
*   **ADD_REG_REG**: `ADD <register1>, <register2>`
*   **SUB_REG_REG**: `SUB <register1>, <register2>`
*   **JMP**: `JMP <address>`
*   **CMP**: `CMP <arg1>, <arg2>` (where `arg1` is a register, `arg2` can be a register or a value)
*   **JE**: `JE <address>`
*   **JNE**: `JNE <address>`

## Registers

| Register | ID | Description                                |
|----------|----|--------------------------------------------|
| `AX`     | 0  | General purpose register                   |
| `BX`     | 1  | General purpose register                   |
| `CX`     | 2  | General purpose register                   |
| `DX`     | 3  | General purpose register                   |
| `SP`     | 4  | Stack pointer (not yet actively used)      |
| `IP`     | 5  | Instruction pointer (current instruction address)|
| `FLAGS`  | 6  | Flags register (0: not equal, 1: equal)    |
