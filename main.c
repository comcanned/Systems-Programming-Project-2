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

    // Open the input file for reading
    FILE* inputFile = fopen(filename, "r");
    if (!inputFile) {
        printf("Error: Could not open file %s\n", filename);
        return 1;
    }

    int LOCCTR = 0;

    // Run Pass 1: Generate Symbol Table
   if (!pass1_generate_symbol_table(inputFile, &LOCCTR)) {
    printf("Assembly error detected in Pass 1. Object file was not created.\n");
    fclose(inputFile);
    return 1;
}


    // If only Pass 1 is requested, print the symbol table and exit
    if (pass1_only) {
        print_symbol_table();
        fclose(inputFile);
        return 0;
    }

    // Run Pass 2: Generate Object Code
   if (!pass2_generate_object_code(inputFile, LOCCTR, filename)) {
    printf("Assembly error detected in Pass 2. Object file was not created.\n");
    fclose(inputFile);
    return 1;
}


    // Close the input file after both passes are complete
    fclose(inputFile);
    return 0;
}
