#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h" // For the parse function
#include "vm_defs.h" // For instruction/register enums (though not directly used in assembler.c logic)

// Declare external memory and registers from vm.c
extern unsigned short memory[65536];
extern unsigned short registers[NUM_REGISTERS];
extern void run_vm();

int main(int argc, char* argv[]) {
    // Check for correct usage
    if (argc != 2) {
        printf("Usage: %s <program.txt>\n", argv[0]);
        return 1;
    }

    // Open the program file
    FILE* program_file = fopen(argv[1], "r");
    if (program_file == NULL) {
        printf("Error: Could not open file %s\n", argv[1]);
        return 1;
    }

    // Initialize all registers to 0.
    for (int i = 0; i < NUM_REGISTERS; i++) {
        registers[i] = 0;
    }

    // Initialize all memory to 0.
    for (int i = 0; i < 65536; i++) {
        memory[i] = 0;
    }

    char error_message[256];
    if (!parse(program_file, memory, error_message)) {
        printf("Assembly Error: %s\n", error_message);
        fclose(program_file);
        return 1;
    }
    fclose(program_file);

    printf("Program assembled successfully. Virtual machine initialized.\n");
    run_vm();

    return 0;
}
