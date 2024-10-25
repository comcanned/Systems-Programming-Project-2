#include "assembler.h"

// Define and initialize the symbol table and counter
Symbol symbolTable[MAX_SYMBOLS];
int symbolCount = 0;

// Function to insert a symbol into the symbol table
void insert_symbol(char* label, int LOCCTR, int line_num) {
    // Check for symbol table overflow
    if (symbolCount >= MAX_SYMBOLS) {
        printf("ASSEMBLY ERROR:\n");
        printf("Line %d: Symbol table overflow\n", line_num);
        exit(1);
    }

    // Check if symbol is already defined
    if (search_symbol(label) != -1) {
        printf("ASSEMBLY ERROR:\n");
        printf("Line %d: Duplicate symbol '%s'\n", line_num, label);
        exit(1);
    }

    // Insert the symbol into the table
    strncpy(symbolTable[symbolCount].symbol, label, sizeof(symbolTable[symbolCount].symbol) - 1);
    symbolTable[symbolCount].symbol[sizeof(symbolTable[symbolCount].symbol) - 1] = '\0'; // Ensure null-termination
    symbolTable[symbolCount].address = LOCCTR;
    symbolCount++;

    // Debug output
    printf("Debug: Inserted symbol '%s' with address %04X at line %d\n", label, LOCCTR, line_num);
}

// Function to search for a symbol in the symbol table
int search_symbol(char* label) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].symbol, label) == 0) {
            return i;  // Return index if found
        }
    }
    return -1;  // Symbol not found
}

// Function to retrieve the address of a symbol (helper for object code generation)
int get_symbol_address(char* label, int line_num) {
    int index = search_symbol(label);
    if (index == -1) {
        printf("ASSEMBLY ERROR:\n");
        printf("Line %d: Undefined symbol '%s'\n", line_num, label);
        exit(1);
    }
    return symbolTable[index].address;
}

// Function to print the symbol table (useful for --pass1only option)
void print_symbol_table() {
    printf("\nSymbol Table:\n");
    printf("Symbol\tAddress\n");
    printf("------\t-------\n");
    for (int i = 0; i < symbolCount; i++) {
        printf("%-6s\t%04X\n", symbolTable[i].symbol, symbolTable[i].address);
    }
}
