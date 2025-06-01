# Welcome to the flewittPC_SDK

This SDK contains 3 tools to program flewitt's PC.
1) Compiler
2) Assembler
3) Emulator

The compiler is for a pseudo-macro language that I (Unbox101) created. It helps making labels to jmp to like C goto; statements.
The assembler executes Opcodes shown in the Instructions table below.
The emulator emulates.

# Using the SDK

Most of the SDK is inside "main.c".
The SDK uses "nob.h" as its build system. It is a bootstrappable build system meaning you only need to compile it for your platform once. From there, you just need to run it to rebuild your program.
Input and output files are inside of "workspace/" directory by default. This directory name can be changed at the top of "main.c".

# Steps

1) Open a console of your choice.
2) Run the command at the top of the "build.c" file to bootstrap the build system.
3) Run the "build" program using your platform specific method whenever you want to build. "build.exe" for windows. "./build" for linux.
4) Now edit either "workspace/input.c" or "workspace/input.asm" and write some programs in it.
5) Run the "main" program to see the help menu. Use the help menu to decide how you want to program and which arguments will let you do that.

# Computer Description

register a: 8bits
register b: 8bits
register j: 12bits (it's special)

# Instructions

| Opcode | Byte | Operands | Cycles | Description                                                |
| :----- | :--: | :------: | :----: | ---------------------------------------------------------- |
| nop    |  0   |    -     |   ?    | No operation.                                              |
| add    |  1   |    -     |   ?    | Add registers a and b. Outputs to register a.              |
| sub    |  2   |    -     |   ?    | Sub registers a and b. Outputs to register a.              |
| load   |  3   |   byte   |   ?    | Load the next constant into register a.                    |
| loadj  |  4   |  short   |   ?    | Load the next 2 constants into jump-register.              |
| fetch  |  5   |    -     |   ?    | Load memory from jump-register address into register a.    |
| fetchj |  6   |    -     |   ?    | Load memory from jump-register address into jump-register. |
| write  |  7   |    -     |   ?    | Write memory from register a to jump-register address.     |
| copyab |  8   |    -     |   ?    | Copy register a to register b.                             |
| copyba |  9   |    -     |   ?    | Copy register b to register a.                             |
| jmp    |  10  |    -     |   ?    | Copies jump-register address into program_counter.         |
| jz     |  11  |    -     |   ?    | "jmp" if register a is zero.                               |
| jc     |  12  |    -     |   ?    | "jmp" if the register a + register b > 255.                |
| writed |  13  |    -     |   ?    | Write the next constant into the LCD display.              |
| readd  |  14  |    -     |   ?    | Copy LCD display output into register a.                   |
| halt   | 255  |    -     |   ?    | Stops the computer.                                        |

# Debug instructions (used in the emulator ONLY):

| Opcode           | Byte | Operand | Cycles | Description                                                                                                                                                          |
| ---------------- | :--: | :-----: | :----: | -------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| _debug_print_mem | 253  |  short  |   ?    | Used in the emulator. Prints the memory at the operand's memory location to console. <br>Also temporarily halts the program. Waits for a keypress before continuing. |
| _debug_halt      | 254  |    -    |   ?    | Used in the emulator. Temporarily halts the program. Waits for a keypress before continuing.                                                                         |

