#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 100
#define MAX_SYMBOLS 100

// Structure for storing symbols in the symbol table
typedef struct {
    char label[10];
    int address;
} Symbol;

// External symbol table and counter for symbol management
extern Symbol symbolTable[MAX_SYMBOLS];
extern int symbolCount;

// Function prototypes for assembler passes (updated to return int)
int pass1_generate_symbol_table(FILE* inputFile, int* LOCCTR);
int pass2_generate_object_code(FILE* inputFile, int LOCCTR, const char* filename);

// Function prototypes for processing lines and generating object code
int parse_line(char* line, int* LOCCTR, int line_num);  // Ensure parse_line also returns int
int generate_object_code(const char* line, int line_num, FILE* objectFile);  // Ensure int return

// Other function prototypes remain unchanged
void handle_directive(const char* opcode, const char* operand, int* LOCCTR, int line_num);
void generate_data_code(const char* opcode, const char* operand, FILE* objectFile);

// Utility function prototypes
int get_opcode_value(const char* opcode);
int is_opcode(const char* opcode);
int is_directive(const char* word);
int calculate_byte_size(const char* operand);
void trim_whitespace(char* str);
int strcasecmp(const char* s1, const char* s2);

// Symbol table function prototypes
int insert_symbol(const char* label, int address, int line_num);
int search_symbol(const char* label);
void print_symbol_table();
int get_symbol_address(const char* label);

#endif // ASSEMBLER_H
