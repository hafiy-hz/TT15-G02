# TT15-G02 - CCP6124 VM Assignment 
Final Work Split 
Member 1(syafiq) — Core Architecture & Data Structures

Register, GeneralRegister, FlagRegister, Memory, PC, SI
Custom MyStack and MyList (no STL)
Draft initial class diagram
Rubric coverage: encapsulation, 1 inheritance hierarchy, 1 of the 3 required DS

Member 2(harith) — Arithmetic, Flags, Instruction base

Instruction abstract base class (team-agreed signature)
MOV, ADD, SUB, MUL, DIV, INC, DEC, RESET
ALL flag-setting logic (OF/UF/CF/ZF) — centralized here for consistency
Sample Program #1 (e.g. factorial of 4)
Rubric coverage: 2nd inheritance hierarchy (Instruction→subclasses), flags 2 marks, 1 of 3 programs

Member 3(hafiy) — Bit Ops, Memory Access, Queue (src folder)

ROL, ROR, SHL, SHR (binary conversion logic)
LOAD/STORE (both addressing modes each)
Custom MyQueue
Sample Program #2 (e.g. average of 4 values)
Rubric coverage: last of 3 required DS, 1 of 3 programs

Member 4(aiden) — I/O, Stack Ops, Runner, Integration, Compile Testing

INPUT, DISPLAY, PUSH, POP
Runner (file parsing → instruction dispatch — main polymorphism point) + CPU::dump()
Final integration of all 4 members into single file
Verify command-line compile (g++) works clean — 25 marks riding on this
Sample Program #3 (e.g. sum of 5 values)
User manual section of report
Rubric coverage: polymorphism point #1, instruction-correctness testing, 1 of 3 programs, user manual
