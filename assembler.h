#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define maximum sizes
#define MAX_SYMBOLS 100
#define MAX_LINE_LENGTH 100

// Symbol table structure
typedef struct {
    char symbol[10];  // Symbol name
    int address;      // Address in hexadecimal
} Symbol;

// Global symbol table and counter
extern Symbol symbolTable[MAX_SYMBOLS];
extern int symbolCount;

// Function prototypes

// Pass 1: Generate Symbol Table
void pass1_generate_symbol_table(FILE* inputFile, int* LOCCTR);

// Pass 2: Generate Object Code
void pass2_generate_object_code(FILE* inputFile, int LOCCTR, const char* filename);

// Line parsing and handling functions
void parse_line(char* line, int* LOCCTR, int line_num);
void handle_directive(const char* opcode, const char* operand, int* LOCCTR, int line_num);

// Object code generation
void generate_object_code(const char* line, int line_num, FILE* objectFile);
void generate_data_code(const char* opcode, const char* operand, FILE* objectFile);

// Opcode helper functions
int get_opcode_value(const char* opcode);
int is_opcode(const char* opcode);
int is_directive(const char* word);

// Utility functions
int calculate_byte_size(const char* operand);
void trim_whitespace(char* str);
int strcasecmp(const char* s1, const char* s2);

// Opcode & Directive tables
extern char* opcodes[];
extern char* directives[];

#endif // ASSEMBLER_H
