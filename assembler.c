#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"    // For the parse function and related definitions
#include "vm_defs.h"   // For VM instruction definitions and run_vm prototype

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <assembly_file>\n", argv[0]);
        return 1;
    }

    FILE *assembly_file = fopen(argv[1], "r");
    if (assembly_file == NULL) {
        perror("Error opening assembly file");
        return 1;
    }

    // Allocate memory for the VM (65536 words * sizeof(unsigned short))
    // Initialize to all zeros
    unsigned short *memory = (unsigned short *)calloc(65536, sizeof(unsigned short));
    if (memory == NULL) {
        perror("Error allocating VM memory");
        fclose(assembly_file);
        return 1;
    }

    char error_message[256];
    int parse_result = parse(assembly_file, memory, error_message);

    fclose(assembly_file); // Close the file after parsing

    if (parse_result == 0) {
        fprintf(stderr, "Assembly Error: %s\n", error_message);
        free(memory);
        return 1;
    }

    printf("Assembly successful. Executing program...\n");
    run_vm(memory); // Call the VM execution function

    free(memory); // Free the allocated memory
    return 0;
}