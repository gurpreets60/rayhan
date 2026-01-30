# rayhan

This is a virtual machine programmed in C.
It is a 16 bit virtual machine: 65536 different possible values allowed in the registers.
Memory: 65kb.

Registers:
General Purpose Registers: (Addition, subtraction etc.)
AX
BX
CX
DX

SP (Stack pointer)
IP (Instruction pointer)
(And more to be implemented later)

## Editor Usage

The `editor` program allows you to create, edit, and run assembly programs for the `rayhan` VM.

**1. Compilation:**
Compile the editor, parser, and VM source files:
```bash
gcc -o editor vm.c parser.c editor.c
```

**2. Starting the Editor:**
To start the editor with an existing file:
```bash
./editor <filename>
```
Example: `./editor complex.txt`

To start the editor with an empty buffer:
```bash
./editor
```

**3. Editor Commands:**
Once inside the editor, you'll see the current file content and a prompt. Enter one of the following commands:

*   `l`: Redisplay the current buffer content.
*   `e <line_number>`: Edit the line at the specified `<line_number>`. You will be prompted to enter new text for that line.
*   `i <line_number>`: Insert a new line before the specified `<line_number>`. You will be prompted to enter the text for the new line.
*   `d <line_number>`: Delete the line at the specified `<line_number>`.
*   `:run`: Assemble and execute the current program in the buffer using the VM. Displays VM output and any assembly errors.
*   `:w`: Save the current buffer to the file it was originally loaded from. If no file was loaded, it will prompt you to use `:w <filename>`.
*   `:w <filename>`: Save the current buffer to the specified `<filename>`.
*   `:e <filename>`: Load the content of the specified `<filename>` into the editor's buffer, replacing the current content.
*   `:q`: Quit the editor.
