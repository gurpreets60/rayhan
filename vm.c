#include <stdio.h>
#include <stdlib.h>
#include "vm_defs.h"

// --- Virtual Machine ---
// This is a simple 16-bit virtual machine.
// It has a 65kb memory and a few registers.

// Memory: 65kb (65536 bytes)
unsigned short memory[65536];

// We can store the register values in an array.
unsigned short registers[NUM_REGISTERS];


void run_vm() {
    printf("Virtual machine starting execution...\n");

    // Fetch-decode-execute cycle
    int running = 1;
    while (running) {
        // Fetch the instruction pointed to by IP
        unsigned short instruction = memory[registers[IP]];

        // Handle IP increment in a single place unless a jump occurs
        int ip_incremented = 0;

        // Decode and execute the instruction
        switch (instruction) {
            case HLT:
                running = 0;
                printf("HLT instruction encountered. Halting.\n");
                break;
            case MOV: {
                unsigned short reg = memory[registers[IP] + 1];
                unsigned short val = memory[registers[IP] + 2];
                registers[reg] = val;
                registers[IP] += 3;
                ip_incremented = 1;
                break;
            }
            case ADD: {
                unsigned short reg = memory[registers[IP] + 1];
                unsigned short val = memory[registers[IP] + 2];
                registers[reg] += val;
                registers[IP] += 3;
                ip_incremented = 1;
                break;
            }
            case SUB: {
                unsigned short reg = memory[registers[IP] + 1];
                unsigned short val = memory[registers[IP] + 2];
                registers[reg] -= val;
                registers[IP] += 3;
                ip_incremented = 1;
                break;
            }
            case MOV_REG_REG: {
                unsigned short reg1 = memory[registers[IP] + 1];
                unsigned short reg2 = memory[registers[IP] + 2];
                registers[reg1] = registers[reg2];
                registers[IP] += 3;
                ip_incremented = 1;
                break;
            }
            case ADD_REG_REG: {
                unsigned short reg1 = memory[registers[IP] + 1];
                unsigned short reg2 = memory[registers[IP] + 2];
                registers[reg1] += registers[reg2];
                registers[IP] += 3;
                ip_incremented = 1;
                break;
            }
            case SUB_REG_REG: {
                unsigned short reg1 = memory[registers[IP] + 1];
                unsigned short reg2 = memory[registers[IP] + 2];
                registers[reg1] -= registers[reg2];
                registers[IP] += 3;
                ip_incremented = 1;
                break;
            }
            case JMP: {
                unsigned short address = memory[registers[IP] + 1];
                registers[IP] = address; // Unconditional jump
                ip_incremented = 1;
                break;
            }
            case CMP: { // CMP reg, val or CMP reg, reg (parser will handle this by emitting correct args)
                unsigned short arg1_type = memory[registers[IP] + 1]; // 0 for reg, 1 for val (conceptual, parser handles actual values)
                unsigned short arg1_val = memory[registers[IP] + 2];
                unsigned short arg2_type = memory[registers[IP] + 3];
                unsigned short arg2_val = memory[registers[IP] + 4];

                unsigned short val1, val2;

                if (arg1_type == 0) val1 = registers[arg1_val]; // It's a register ID
                else val1 = arg1_val; // It's an immediate value

                if (arg2_type == 0) val2 = registers[arg2_val]; // It's a register ID
                else val2 = arg2_val; // It's an immediate value

                if (val1 == val2) {
                    registers[FLAGS] = 1; // Set Equal flag
                } else {
                    registers[FLAGS] = 0; // Clear Equal flag
                }
                registers[IP] += 5; // CMP takes 4 arguments: instruction + arg1_type + arg1_val + arg2_type + arg2_val
                ip_incremented = 1;
                break;
            }
            case JE: {
                unsigned short address = memory[registers[IP] + 1];
                if (registers[FLAGS] == 1) { // If Equal flag is set
                    registers[IP] = address;
                } else {
                    registers[IP] += 2; // Skip address argument
                }
                ip_incremented = 1;
                break;
            }
            case JNE: {
                unsigned short address = memory[registers[IP] + 1];
                if (registers[FLAGS] == 0) { // If Equal flag is NOT set
                    registers[IP] = address;
                } else {
                    registers[IP] += 2; // Skip address argument
                }
                ip_incremented = 1;
                break;
            }
            default:
                printf("Unknown instruction: %d at address %d\n", instruction, registers[IP]);
                running = 0;
                break;
        }

        // Error prevention: If IP wasn't incremented by an instruction (e.g., in HLT)
        if (!ip_incremented && running) {
            registers[IP]++;
        }
    }

    printf("Execution finished.\n");
    printf("Final value of AX: %d\n", registers[AX]);
    printf("Final value of BX: %d\n", registers[BX]);
    printf("Final value of CX: %d\n", registers[CX]);
    printf("Final value of FLAGS: %d\n", registers[FLAGS]);
}

