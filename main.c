#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assembler.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filename> [--pass1only]\n", argv[0]);
        return 1;
    }

    const char* filename = argv[1];
    int pass1_only = (argc == 3 && strcmp(argv[2], "--pass1only") == 0);

    FILE* inputFile = fopen(filename, "r");
    if (!inputFile) {
        printf("Error: Could not open file %s\n", filename);
        return 1;
    }

    int LOCCTR = 0;

    // Pass 1: Generate Symbol Table
    pass1_generate_symbol_table(inputFile, &LOCCTR);

    if (pass1_only) {
        // Output symbol table if only running Pass 1
        print_symbol_table();
        fclose(inputFile);
        return 0;
    }

    // Pass 2: Generate Object Code
    pass2_generate_object_code(inputFile, LOCCTR, filename);

    fclose(inputFile);
    return 0;
}
