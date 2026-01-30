#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // For isdigit
#include "parser.h"
#include "vm_defs.h"

// Helper function to check if a string is a number
static int is_number(char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i])) {
            return 0;
        }
    }
    return 1;
}

// Declare external memory and registers from vm.c
extern unsigned short memory[65536];
extern unsigned short registers[NUM_REGISTERS];
extern void run_vm();

// A simple linked list to store lines of text
typedef struct Line {
    char* text;
    struct Line* next;
} Line;

Line* head = NULL;
char* current_filename = NULL;

// Function to free all lines
void free_lines() {
    Line* current = head;
    while (current != NULL) {
        Line* next = current->next;
        free(current->text);
        free(current);
        current = next;
    }
    head = NULL;
}

// Helper function to get line count
int get_line_count() {
    int count = 0;
    Line* current = head;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

// Helper function to get a line by its number
Line* get_line(int line_num) {
    if (line_num <= 0) return NULL;
    Line* current = head;
    for (int i = 1; i < line_num && current != NULL; i++) {
        current = current->next;
    }
    return current;
}

// Helper function to delete a line
void delete_line(int line_num) {
    if (line_num <= 0 || line_num > get_line_count()) {
        printf("Invalid line number.\n");
        return;
    }

    if (line_num == 1) {
        Line* temp = head;
        head = head->next;
        free(temp->text);
        free(temp);
        printf("Line %d deleted.\n", line_num);
        return;
    }

    Line* prev = get_line(line_num - 1);
    if (prev == NULL || prev->next == NULL) {
        printf("Error deleting line.\n");
        return;
    }

    Line* temp = prev->next;
    prev->next = temp->next;
    free(temp->text);
    free(temp);
    printf("Line %d deleted.\n", line_num);
}

// Helper function to insert a line
void insert_line(int line_num, const char* text) {
    if (line_num <= 0 || line_num > get_line_count() + 1) {
        printf("Invalid line number for insertion.\n");
        return;
    }

    Line* new_line = (Line*)malloc(sizeof(Line));
    // Ensure the new line has a newline character
    if (text[strlen(text) - 1] != '\n') {
        char* temp_text = (char*)malloc(strlen(text) + 2);
        strcpy(temp_text, text);
        strcat(temp_text, "\n");
        new_line->text = temp_text;
    } else {
        new_line->text = strdup(text);
    }
    new_line->next = NULL;

    if (line_num == 1) {
        new_line->next = head;
        head = new_line;
    } else {
        Line* prev = get_line(line_num - 1);
        if (prev == NULL) {
            printf("Error inserting line.\n");
            free(new_line->text);
            free(new_line);
            return;
        }
        new_line->next = prev->next;
        prev->next = new_line;
    }
    printf("Line %d inserted.\n", line_num);
}


// Helper function to replace a line
void replace_line(int line_num, const char* new_text) {
    Line* target_line = get_line(line_num);
    if (target_line == NULL) {
        printf("Invalid line number.\n");
        return;
    }

    free(target_line->text);
    // Ensure the new line has a newline character
    if (new_text[strlen(new_text) - 1] != '\n') {
        char* temp_text = (char*)malloc(strlen(new_text) + 2);
        strcpy(temp_text, new_text);
        strcat(temp_text, "\n");
        target_line->text = temp_text;
    } else {
        target_line->text = strdup(new_text);
    }
    printf("Line %d replaced.\n", line_num);
}


// Function to load a file into the buffer
void load_file(const char* filename) {
    free_lines(); // Clear existing buffer
    if (current_filename != NULL) {
        free(current_filename);
        current_filename = NULL;
    }

    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error: Could not open file %s. Starting with empty buffer.\n", filename);
        current_filename = strdup(filename); // Store filename even if empty
        return;
    }

    current_filename = strdup(filename);
    char buffer[256];
    Line* current = NULL;
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        Line* new_line = (Line*)malloc(sizeof(Line));
        new_line->text = strdup(buffer);
        new_line->next = NULL;

        if (head == NULL) {
            head = new_line;
        } else {
            current->next = new_line;
        }
        current = new_line;
    }
    fclose(fp);
    printf("Loaded %s\n", filename);
}

// Function to save the buffer to a file
void save_file(const char* filename) {
    if (filename == NULL) {
        printf("No filename specified. Use :w <filename>\n");
        return;
    }

    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error: Could not save file %s\n", filename);
        return;
    }

    Line* current = head;
    while (current != NULL) {
        fputs(current->text, fp);
        current = current->next;
    }
    fclose(fp);
    printf("Saved to %s\n", filename);
}

// Function to display the buffer
void display_buffer() {
    printf("--- Current Buffer (%s) ---\n", current_filename ? current_filename : "[untitled]");
    Line* current = head;
    int line_num = 1;
    while (current != NULL) {
        printf("%4d: %s", line_num, current->text);
        current = current->next;
        line_num++;
    }
    printf("---------------------------\n");
}

// Function to run the current buffer through the VM
void run_buffer() {
    if (head == NULL) {
        printf("Buffer is empty. Nothing to run.\n");
        return;
    }

    // Create a temporary file to write the buffer content
    FILE* temp_fp = fopen("/tmp/editor_temp_program.txt", "w");
    if (temp_fp == NULL) {
        printf("Error: Could not create temporary file for execution.\n");
        return;
    }
    Line* current = head;
    while (current != NULL) {
        fputs(current->text, temp_fp);
        current = current->next;
    }
    fclose(temp_fp);

    // Initialize VM registers and memory (reset state for each run)
    for (int i = 0; i < NUM_REGISTERS; i++) {
        registers[i] = 0;
    }
    for (int i = 0; i < 65536; i++) {
        memory[i] = 0;
    }

    // Parse the temporary file
    FILE* program_file = fopen("/tmp/editor_temp_program.txt", "r");
    char error_message[256];
    if (!parse(program_file, memory, error_message)) {
        printf("Assembly Error: %s\n", error_message);
        fclose(program_file);
        remove("/tmp/editor_temp_program.txt"); // Clean up temp file
        return;
    }
    fclose(program_file);
    remove("/tmp/editor_temp_program.txt"); // Clean up temp file

    printf("Program assembled successfully.\n");
    run_vm(); // Execute the program
}


int main(int argc, char* argv[]) {
    if (argc == 2) {
        load_file(argv[1]);
    } else {
        printf("Starting editor with empty buffer. Use :e <filename> to load.\n");
    }

    char command_line[256];
    int running = 1;

    while (running) {
        display_buffer();
        printf("Enter command (l, e <num>, i <num>, d <num>, :run, :w, :q): ");
        if (fgets(command_line, sizeof(command_line), stdin) == NULL) {
            break;
        }

        // Remove trailing newline
        if (command_line[strlen(command_line) - 1] == '\n') {
            command_line[strlen(command_line) - 1] = '\0';
        }

        char* cmd_token = strtok(command_line, " ");

        if (cmd_token == NULL) {
            continue;
        }

        if (strcmp(cmd_token, "l") == 0) {
            // display_buffer already called
        } else if (strcmp(cmd_token, ":q") == 0) {
            running = 0;
        } else if (strcmp(cmd_token, ":run") == 0) {
            run_buffer();
        } else if (strcmp(cmd_token, ":w") == 0) {
            char* filename = strtok(NULL, " ");
            if (filename != NULL) {
                save_file(filename);
                if (current_filename != NULL) free(current_filename);
                current_filename = strdup(filename);
            } else {
                save_file(current_filename);
            }
        } else if (strcmp(cmd_token, ":e") == 0) {
            char* filename = strtok(NULL, " ");
            if (filename != NULL) {
                load_file(filename);
            } else {
                printf("Usage: :e <filename>\n");
            }
        } else if (strcmp(cmd_token, "d") == 0) {
            char* line_num_str = strtok(NULL, " ");
            if (line_num_str != NULL && is_number(line_num_str)) {
                int line_num = atoi(line_num_str);
                delete_line(line_num);
            } else {
                printf("Usage: d <line_number>\n");
            }
        } else if (strcmp(cmd_token, "i") == 0) {
            char* line_num_str = strtok(NULL, " ");
            if (line_num_str != NULL && is_number(line_num_str)) {
                int line_num = atoi(line_num_str);
                printf("Enter text to insert: ");
                char text_buffer[256];
                if (fgets(text_buffer, sizeof(text_buffer), stdin) != NULL) {
                    insert_line(line_num, text_buffer);
                }
            } else {
                printf("Usage: i <line_number>\n");
            }
        } else if (strcmp(cmd_token, "e") == 0) {
            char* line_num_str = strtok(NULL, " ");
            if (line_num_str != NULL && is_number(line_num_str)) {
                int line_num = atoi(line_num_str);
                Line* line_to_edit = get_line(line_num);
                if (line_to_edit != NULL) {
                    printf("Current text (Line %d): %s", line_num, line_to_edit->text);
                    printf("Enter new text: ");
                    char text_buffer[256];
                    if (fgets(text_buffer, sizeof(text_buffer), stdin) != NULL) {
                        replace_line(line_num, text_buffer);
                    }
                }
            } else {
                printf("Usage: e <line_number>\n");
            }
        }
        else {
            printf("Unknown command.\n");
        }
    }

    free_lines();
    if (current_filename != NULL) {
        free(current_filename);
    }
    return 0;
}
