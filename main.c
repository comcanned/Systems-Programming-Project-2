#include "assembler.h"

Symbol symbolTable[MAX_SYMBOLS];
int symbolCount = 0;

// Insert a symbol into the symbol table
int insert_symbol(const char* label, int address, int line_num) {
    // Check for duplicate symbol
    if (search_symbol(label) != -1) {
        printf("ASSEMBLY ERROR: Duplicate symbol '%s' at line %d\n", label, line_num);
        return 0;  // Return failure due to duplicate symbol
    }

    // Check for symbol table overflow
    if (symbolCount >= MAX_SYMBOLS) {
        printf("ASSEMBLY ERROR: Symbol table overflow at line %d\n", line_num);
        exit(1);  // Exit due to exceeding symbol table size
    }

    // Insert symbol into the symbol table
    strncpy(symbolTable[symbolCount].label, label, sizeof(symbolTable[symbolCount].label) - 1);
    symbolTable[symbolCount].label[sizeof(symbolTable[symbolCount].label) - 1] = '\0';  // Ensure null-terminated
    symbolTable[symbolCount].address = address;
    symbolCount++;
    return 1;  // Return success
}

// Search for a symbol in the symbol table by label
int search_symbol(const char* label) {
    for (int i = 0; i < symbolCount; i++) {
        if (strcmp(symbolTable[i].label, label) == 0) {
            return symbolTable[i].address;
        }
    }
    return -1;  // Return -1 if symbol not found
}

// Retrieve the address of a symbol in the symbol table
int get_symbol_address(const char* label) {
    return search_symbol(label);  // Uses search_symbol to find address
}

// Print the contents of the symbol table (used for --pass1only option)
void print_symbol_table() {
    printf("Symbol Table:\n");
    printf("Label      Address\n");
    for (int i = 0; i < symbolCount; i++) {
        printf("%-10s %04X\n", symbolTable[i].label, symbolTable[i].address);
    }
}
