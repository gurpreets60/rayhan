#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "parser.h"
#include "vm_defs.h"

// Helper function to get register ID from string
int get_reg_id(char* reg_str) {
    // Convert to uppercase for case-insensitive matching
    char upper_reg_str[10]; // Max 4 chars for reg name + null terminator
    int i;
    for (i = 0; reg_str[i] != '\0' && i < 9; i++) {
        upper_reg_str[i] = toupper((unsigned char)reg_str[i]);
    }
    upper_reg_str[i] = '\0';

    if (strcmp(upper_reg_str, "AX") == 0) return AX;
    if (strcmp(upper_reg_str, "BX") == 0) return BX;
    if (strcmp(upper_reg_str, "CX") == 0) return CX;
    if (strcmp(upper_reg_str, "DX") == 0) return DX;
    if (strcmp(upper_reg_str, "SP") == 0) return SP;
    if (strcmp(upper_reg_str, "IP") == 0) return IP;
    if (strcmp(upper_reg_str, "FLAGS") == 0) return FLAGS;
    return -1;
}

// Helper function to check if a string is a number
static int is_number(char* str) {
    if (str == NULL || *str == '\0') return 0;
    char* p = str;
    if (*p == '-') p++; // Allow negative numbers
    while (*p != '\0') {
        if (!isdigit((unsigned char)*p)) {
            return 0;
        }
        p++;
    }
    return 1;
}

// Helper function to trim leading/trailing whitespace
char* trim_whitespace(char* str) {
    char* end;
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0) return str;
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}


int parse(FILE* source, unsigned short* memory, char* error_message) {
    char line[256];
    int line_num = 0;
    int mem_ptr = 0;

    char instruction_str[32];
    char arg1_str[32];
    char arg2_str[32];

    while (fgets(line, sizeof(line), source)) {
        line_num++;

        // Remove comments
        char* comment_start = strstr(line, "//");
        if (comment_start != NULL) {
            *comment_start = '\0';
        }

        // Trim whitespace
        char* trimmed_line = trim_whitespace(line);

        if (strlen(trimmed_line) == 0) continue; // Skip empty lines

        // Try to parse the instruction and arguments
        // Use sscanf to extract parts of the line
        int num_parsed = sscanf(trimmed_line, "%s %31[^,],%s", instruction_str, arg1_str, arg2_str);
        
        printf("DEBUG: Line %d, Trimmed_line: '%s'\n", line_num, trimmed_line);
        printf("DEBUG: num_parsed: %d\n", num_parsed);
        printf("DEBUG: instruction_str: '%s'\n", instruction_str);
        if (num_parsed >= 2) printf("DEBUG: arg1_str: '%s'\n", arg1_str);
        if (num_parsed >= 3) printf("DEBUG: arg2_str: '%s'\n", arg2_str);
        fflush(stdout);
        
        // Convert instruction_str to uppercase for case-insensitive matching
        for (int i = 0; instruction_str[i]; i++) {
            instruction_str[i] = toupper((unsigned char)instruction_str[i]);
        }

        
        if (num_parsed < 1) { // No instruction found
            sprintf(error_message, "Line %d: Syntax error or empty line after trimming.", line_num);
            return 0;
        }

        if (strcmp(instruction_str, "HLT") == 0) {
            memory[mem_ptr++] = HLT;
        } else if (strcmp(instruction_str, "MOV") == 0) {
            if (num_parsed < 3) {
                sprintf(error_message, "Line %d: MOV requires two arguments: MOV <reg>, <val/reg>.", line_num);
                return 0;
            }
            int reg1_id = get_reg_id(trim_whitespace(arg1_str));
            if (reg1_id == -1) {
                sprintf(error_message, "Line %d: Invalid register '%s' for MOV.", line_num, arg1_str);
                return 0;
            }
            if (is_number(trim_whitespace(arg2_str))) {
                memory[mem_ptr++] = MOV;
                memory[mem_ptr++] = reg1_id;
                memory[mem_ptr++] = atoi(trim_whitespace(arg2_str));
            } else {
                int reg2_id = get_reg_id(trim_whitespace(arg2_str));
                if (reg2_id == -1) {
                    sprintf(error_message, "Line %d: Invalid argument '%s' for MOV (not a number or register).", line_num, arg2_str);
                    return 0;
                }
                memory[mem_ptr++] = MOV_REG_REG;
                memory[mem_ptr++] = reg1_id;
                memory[mem_ptr++] = reg2_id;
            }
        } else if (strcmp(instruction_str, "ADD") == 0) {
            if (num_parsed < 3) {
                sprintf(error_message, "Line %d: ADD requires two arguments: ADD <reg>, <val/reg>.", line_num);
                return 0;
            }
            int reg1_id = get_reg_id(trim_whitespace(arg1_str));
            if (reg1_id == -1) {
                sprintf(error_message, "Line %d: Invalid register '%s' for ADD.", line_num, arg1_str);
                return 0;
            }
            if (is_number(trim_whitespace(arg2_str))) {
                memory[mem_ptr++] = ADD;
                memory[mem_ptr++] = reg1_id;
                memory[mem_ptr++] = atoi(trim_whitespace(arg2_str));
            } else {
                int reg2_id = get_reg_id(trim_whitespace(arg2_str));
                if (reg2_id == -1) {
                    sprintf(error_message, "Line %d: Invalid argument '%s' for ADD (not a number or register).", line_num, arg2_str);
                    return 0;
                }
                memory[mem_ptr++] = ADD_REG_REG;
                memory[mem_ptr++] = reg1_id;
                memory[mem_ptr++] = reg2_id;
            }
        } else if (strcmp(instruction_str, "SUB") == 0) {
            if (num_parsed < 3) {
                sprintf(error_message, "Line %d: SUB requires two arguments: SUB <reg>, <val/reg>.", line_num);
                return 0;
            }
            int reg1_id = get_reg_id(trim_whitespace(arg1_str));
            if (reg1_id == -1) {
                sprintf(error_message, "Line %d: Invalid register '%s' for SUB.", line_num, arg1_str);
                return 0;
            }
            if (is_number(trim_whitespace(arg2_str))) {
                memory[mem_ptr++] = SUB;
                memory[mem_ptr++] = reg1_id;
                memory[mem_ptr++] = atoi(trim_whitespace(arg2_str));
            } else {
                int reg2_id = get_reg_id(trim_whitespace(arg2_str));
                if (reg2_id == -1) {
                    sprintf(error_message, "Line %d: Invalid argument '%s' for SUB (not a number or register).", line_num, arg2_str);
                    return 0;
                }
                memory[mem_ptr++] = SUB_REG_REG;
                memory[mem_ptr++] = reg1_id;
                memory[mem_ptr++] = reg2_id;
            }
        } else if (strcmp(instruction_str, "JMP") == 0) {
            if (num_parsed < 2 || !is_number(trim_whitespace(arg1_str))) {
                sprintf(error_message, "Line %d: JMP requires a numeric address.", line_num);
                return 0;
            }
            memory[mem_ptr++] = JMP;
            memory[mem_ptr++] = atoi(trim_whitespace(arg1_str));
        } else if (strcmp(instruction_str, "CMP") == 0) {
            if (num_parsed < 3) {
                sprintf(error_message, "Line %d: CMP requires two arguments: CMP <reg>, <val/reg>.", line_num);
                return 0;
            }
            int reg1_id = get_reg_id(trim_whitespace(arg1_str));
            if (reg1_id == -1) {
                sprintf(error_message, "Line %d: Invalid register '%s' for first argument of CMP.", line_num, arg1_str);
                return 0;
            }
            memory[mem_ptr++] = CMP;
            memory[mem_ptr++] = 0; // arg1 is always a register
            memory[mem_ptr++] = reg1_id;

            if (is_number(trim_whitespace(arg2_str))) {
                memory[mem_ptr++] = 1; // arg2 is an immediate value
                memory[mem_ptr++] = atoi(trim_whitespace(arg2_str));
            } else {
                int reg2_id = get_reg_id(trim_whitespace(arg2_str));
                if (reg2_id == -1) {
                    sprintf(error_message, "Line %d: Invalid argument '%s' for second argument of CMP.", line_num, arg2_str);
                    return 0;
                }
                memory[mem_ptr++] = 0; // arg2 is a register
                memory[mem_ptr++] = reg2_id;
            }
        } else if (strcmp(instruction_str, "JE") == 0) {
            if (num_parsed < 2 || !is_number(trim_whitespace(arg1_str))) {
                sprintf(error_message, "Line %d: JE requires a numeric address.", line_num);
                return 0;
            }
            memory[mem_ptr++] = JE;
            memory[mem_ptr++] = atoi(trim_whitespace(arg1_str));
        } else if (strcmp(instruction_str, "JNE") == 0) {
            if (num_parsed < 2 || !is_number(trim_whitespace(arg1_str))) {
                sprintf(error_message, "Line %d: JNE requires a numeric address.", line_num);
                return 0;
            }
            memory[mem_ptr++] = JNE;
            memory[mem_ptr++] = atoi(trim_whitespace(arg1_str));
        } else {
            sprintf(error_message, "Line %d: Unknown instruction '%s'.", line_num, instruction_str);
            return 0;
        }
    }
    return 1; // Success
}

// Temporary main function for sscanf debugging

int main() {
    char test_line[] = "CMP AX, 0";
    char instruction_str[32];
    char arg1_str[32];
    char arg2_str[32];

    char* trimmed_line = trim_whitespace(test_line);

    int num_parsed = sscanf(trimmed_line, "%s %31[^,],%s", instruction_str, arg1_str, arg2_str);

    printf("Test line: '%s'\n", test_line);
    printf("Trimmed line: '%s'\n", trimmed_line);
    printf("num_parsed: %d\n", num_parsed);
    printf("instruction_str: '%s'\n", instruction_str);
    if (num_parsed >= 2) printf("arg1_str: '%s'\n", arg1_str);
    if (num_parsed >= 3) printf("arg2_str: '%s'\n", arg2_str);
    
    // Test with spaces after comma
    char test_line2[] = "CMP BX,  10";
    trimmed_line = trim_whitespace(test_line2);
    num_parsed = sscanf(trimmed_line, "%s %31[^,],%s", instruction_str, arg1_str, arg2_str);
    printf("\nTest line 2: '%s'\n", test_line2);
    printf("Trimmed line 2: '%s'\n", trimmed_line);
    printf("num_parsed: %d\n", num_parsed);
    printf("instruction_str: '%s'\n", instruction_str);
    if (num_parsed >= 2) printf("arg1_str: '%s'\n", arg1_str);
    if (num_parsed >= 3) printf("arg2_str: '%s'\n", arg2_str);
    
    // Test with no comma
    char test_line3[] = "JMP 100";
    trimmed_line = trim_whitespace(test_line3);
    num_parsed = sscanf(trimmed_line, "%s %31[^,],%s", instruction_str, arg1_str, arg2_str);
    printf("\nTest line 3: '%s'\n", test_line3);
    printf("Trimmed line 3: '%s'\n", trimmed_line);
    printf("num_parsed: %d\n", num_parsed);
    printf("instruction_str: '%s'\n", instruction_str);
    if (num_parsed >= 2) printf("arg1_str: '%s'\n", arg1_str);
    if (num_parsed >= 3) printf("arg2_str: '%s'\n", arg2_str);
    
    return 0;
}

