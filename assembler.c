#include "assembler.h"

// Define opcode and directive tables
char* opcodes[] = {
    "ADD", "AND", "COMP", "DIV", "J", "JEQ", "JGT", "JLT", "JSUB",
    "LDA", "LDCH", "LDL", "LDX", "MUL", "OR", "RD", "RSUB", "STA",
    "STCH", "STL", "STSW", "STX", "SUB", "TD", "TIX", "WD"
};

char* directives[] = {
    "START", "END", "BYTE", "WORD", "RESB", "RESW", "BASE"
};

// Pass 1: Generate symbol table
void pass1_generate_symbol_table(FILE* inputFile, int* LOCCTR) {
    char line[MAX_LINE_LENGTH];
    int line_num = 1;

    while (fgets(line, sizeof(line), inputFile)) {
        parse_line(line, LOCCTR, line_num);
        line_num++;
    }
}

// Pass 2: Generate object code
void pass2_generate_object_code(FILE* inputFile, int LOCCTR, const char* filename) {
    char objectFileName[256];
    snprintf(objectFileName, sizeof(objectFileName), "%s.obj", filename);

    FILE* objectFile = fopen(objectFileName, "w");
    if (!objectFile) {
        printf("Error: Could not create object file %s\n", objectFileName);
        return;
    }

    char line[MAX_LINE_LENGTH];
    int line_num = 1;

    // Reset the input file position to the beginning
    rewind(inputFile);

    // Generate object code line by line
    while (fgets(line, sizeof(line), inputFile)) {
        generate_object_code(line, line_num, objectFile);
        line_num++;
    }

    fclose(objectFile);
}

// Parse a line of the SIC assembly file
// Parse a line of the SIC assembly file
void parse_line(char* line, int* LOCCTR, int line_num) {
    char label[10] = "", opcode[10] = "", operand[MAX_LINE_LENGTH] = "";

    // Trim any leading and trailing whitespace from the line
    trim_whitespace(line);

    // If the line is a comment or blank, skip it
    if (line[0] == '#' || line[0] == '\0'){ return;
    }
    // Check if the line starts with a tab or space (indicating no label)
    if (isspace(line[0])) {
        // Parse the opcode and operand directly, skipping the label
        sscanf(line, "%9s %99[^\n]", opcode, operand);
    } else {
        // Parse the label, opcode, and operand
        sscanf(line, "%9s %9s %99[^\n]", label, opcode, operand);
    }

    // Trim the extracted components to remove extra whitespace
    trim_whitespace(label);
    trim_whitespace(opcode);
    trim_whitespace(operand);

    // Debug print the parsed label, opcode, and operand
    printf("Debug: Label='%s', Opcode='%s', Operand='%s'\n", label, opcode, operand);

    // If there's a label, insert it into the symbol table
    if (strlen(label) > 0 && search_symbol(label) == -1) {
        insert_symbol(label, *LOCCTR, line_num);
    }

    // Check if the operand contains indexed addressing (SRC,X)
    if (strchr(operand, ',') != NULL) {
        char symbol[10] = "", index[10] = "";
        sscanf(operand, "%9[^,],%9s", symbol, index); // Split the operand into symbol and index part

        // Debug message for indexed addressing
        printf("Debug: Indexed addressing: Symbol='%s', Index='%s' at line %d\n", symbol, index, line_num);

        // Check if the symbol (e.g., SRC) is valid and present in the symbol table
        if (search_symbol(symbol) == -1) {
            printf("ASSEMBLY ERROR: Undefined symbol '%s' at line %d\n", symbol, line_num);
            exit(1);
        }

        // Ensure the index is 'X' (since only X is valid for indexed addressing in SIC)
        if (strcmp(index, "X") != 0) {
            printf("ASSEMBLY ERROR: Invalid index register '%s' at line %d. Only 'X' is allowed.\n", index, line_num);
            exit(1);
        }

        // Handle the opcode (e.g., LDCH) with indexed addressing
        if (is_opcode(opcode)) {
            *LOCCTR += 3;  // Assume format 3 instructions for SIC
        } else {
            printf("ASSEMBLY ERROR:\n%s\nLine %d: Invalid opcode or directive '%s'\n", line, line_num, opcode);
            exit(1);
        }
    }
    // Handle the BYTE directive
    else if (strcmp(opcode, "BYTE") == 0) {
        if ((operand[0] == 'C' || operand[0] == 'X') && operand[1] == '\'' && operand[strlen(operand) - 1] == '\'') {
            // Valid format: C'...' or X'...'
        } else {
            printf("ASSEMBLY ERROR: Invalid BYTE operand format '%s' at line %d\n", operand, line_num);
            exit(1);
        }
    }
    else if (strcmp(opcode, "BASE") == 0) {
        // Ensure that the BASE directive has an operand
        if (strlen(operand) == 0) {
            printf("ASSEMBLY ERROR: BASE directive requires a label operand at line %d\n", line_num);
            exit(1);
        }

        // Check if the operand (symbol) is in the symbol table
        if (search_symbol(operand) == -1) {
            printf("ASSEMBLY ERROR: Undefined symbol '%s' at line %d\n", operand, line_num);
            exit(1);
        }
    }
    else if (is_opcode(opcode)) {
        *LOCCTR += 3;  // Assume format 3 instructions for SIC
    }
    else if (is_directive(opcode)) {
        if (strcmp(opcode, "START") == 0) {
            *LOCCTR = strtol(operand, NULL, 16);  // Initialize LOCCTR
        } else if (strcmp(opcode, "BYTE") == 0) {
            *LOCCTR += calculate_byte_size(operand);
        } else if (strcmp(opcode, "WORD") == 0) {
            *LOCCTR += 3;  // A WORD is 3 bytes
        } else if (strcmp(opcode, "RESB") == 0) {
            *LOCCTR += atoi(operand);  // Reserve bytes
        } else if (strcmp(opcode, "RESW") == 0) {
            *LOCCTR += 3 * atoi(operand);  // Reserve words (3 bytes each)
        } else if (strcmp(opcode, "END") == 0) {
            return;
        }
    } else {
        printf("ASSEMBLY ERROR:\n%s\nLine %d: Invalid opcode or directive '%s'\n", line, line_num, opcode);
        exit(1);
    }
}



// Handle directives in Pass 1
void handle_directive(const char* opcode, const char* operand, int* LOCCTR, int line_num) {
    if (strcmp(opcode, "START") == 0) {
        *LOCCTR = strtol(operand, NULL, 16); // Initialize LOCCTR based on START operand
    } else if (strcmp(opcode, "BYTE") == 0) {
        *LOCCTR += calculate_byte_size(operand);
    } else if (strcmp(opcode, "WORD") == 0) {
        *LOCCTR += 3;
    } else if (strcmp(opcode, "RESB") == 0) {
        *LOCCTR += atoi(operand);
    } else if (strcmp(opcode, "RESW") == 0) {
        *LOCCTR += 3 * atoi(operand);
    } else if (strcmp(opcode, "BASE") == 0) {
        if (strlen(operand) == 0 || search_symbol(operand) == -1) {
            printf("ASSEMBLY ERROR: BASE directive requires a valid label operand at line %d\n", line_num);
            exit(1);
        }
    }
}

// Generate object code for a line (Pass 2)
void generate_object_code(const char* line, int line_num, FILE* objectFile) {
    char label[10] = "", opcode[10] = "", operand[MAX_LINE_LENGTH] = "";

    sscanf(line, "%9s %9s %99[^\n]", label, opcode, operand);
    trim_whitespace(label);
    trim_whitespace(opcode);
    trim_whitespace(operand);

    if (is_opcode(opcode)) {
        int opcode_value = get_opcode_value(opcode);
        int operand_address = get_symbol_address(operand, line_num);
        fprintf(objectFile, "%02X%04X\n", opcode_value, operand_address);
    } else if (strcmp(opcode, "BYTE") == 0 || strcmp(opcode, "WORD") == 0) {
        generate_data_code(opcode, operand, objectFile);
    } else if (strcmp(opcode, "RESB") == 0 || strcmp(opcode, "RESW") == 0) {
        // Skip RESB and RESW in the object code
    } else {
        printf("ASSEMBLY ERROR:\n%s\nLine %d: Invalid opcode or directive '%s'\n", line, line_num, opcode);
        exit(1);
    }
}

// Additional helper functions like generate_data_code, calculate_byte_size, and others remain as previously defined in your Project 2.


// Generate data code for BYTE and WORD directives
void generate_data_code(const char* opcode, const char* operand, FILE* objectFile) {
    if (strcmp(opcode, "BYTE") == 0) {
        if (operand[0] == 'C') {
            for (int i = 2; i < strlen(operand) - 1; i++) {
                fprintf(objectFile, "%02X", operand[i]);
            }
            fprintf(objectFile, "\n");
        } else if (operand[0] == 'X') {
            fprintf(objectFile, "%s\n", operand + 2);
        }
    } else if (strcmp(opcode, "WORD") == 0) {
        int value = atoi(operand);
        fprintf(objectFile, "%06X\n", value);
    }
}

// Retrieve the numeric value of an opcode
int get_opcode_value(const char* opcode) {
    for (int i = 0; i < sizeof(opcodes) / sizeof(opcodes[0]); i++) {
        if (strcasecmp(opcodes[i], opcode) == 0) {
            return i * 3; // Example calculation for opcode values
        }
    }
    return -1; // Invalid opcode
}

// Check if a word is an opcode
int is_opcode(const char* opcode) {
    for (int i = 0; i < sizeof(opcodes) / sizeof(opcodes[0]); i++) {
        if (strcasecmp(opcodes[i], opcode) == 0) {
            return 1;
        }
    }
    return 0;
}

// Check if a word is a directive
int is_directive(const char* word) {
    for (int i = 0; i < sizeof(directives) / sizeof(directives[0]); i++) {
        if (strcasecmp(directives[i], word) == 0) {
            return 1;
        }
    }
    return 0;
}

// Calculate the size of a BYTE directive
int calculate_byte_size(const char* operand) {
    if (operand[0] == 'C' && operand[1] == '\'' && operand[strlen(operand) - 1] == '\'') {
        return strlen(operand) - 3; // Subtract for 'C' + quotes
    } else if (operand[0] == 'X' && operand[1] == '\'' && operand[strlen(operand) - 1] == '\'') {
        return (strlen(operand) - 3) / 2; // Subtract for 'X' + quotes
    } else {
        printf("ASSEMBLY ERROR: Invalid BYTE operand format '%s'\n", operand);
        exit(1);
    }
    return 0;
}



// Utility functions
void trim_whitespace(char* str) {
    char *end;
    while (isspace((unsigned char)*str)) str++;
    if (*str == '\0') return;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
}

int strcasecmp(const char* s1, const char* s2) {
    while (*s1 && (tolower((unsigned char)*s1) == tolower((unsigned char)*s2))) {
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}
