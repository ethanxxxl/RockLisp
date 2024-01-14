#include <argp.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "scanner.h"
#include "parser.h"
#include "code-generator.h"

struct argp_option g_argp_opts[] = {
    { "output", 'o', "filename", 0, "name of the output file", 0},
    { "loud-scanner", 'S', 0, OPTION_ARG_OPTIONAL, "produce scanner output", 1},
    { "loud-parser",  'P', 0, OPTION_ARG_OPTIONAL, "produce parser output",  1},
    { "loud-asm",     'A', 0, OPTION_ARG_OPTIONAL, "produce code-gen output",1},
    { 0 },
};

struct arguments {
    const char *ifilename;
    const char *ofilename;
    bool print_scanner;
    bool print_parser;
    bool print_asm;
};
error_t arg_parser(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;

    switch (key) {
    case ARGP_KEY_NO_ARGS:
        // error condition
        argp_usage(state);
        break;
    case ARGP_KEY_END:
        if (state->arg_num < 1)
            // not enough arguments
            argp_usage(state);
        break;
    case ARGP_KEY_ARG:
        // set ifilename, unless its already set, then error condition.
        if (arguments->ifilename != NULL)
            argp_usage(state);

        arguments->ifilename = arg;
        break;
    case 'o':
        if (arguments->ofilename != NULL)
            argp_error(state, "extra output file %s", arg);
        arguments->ofilename = arg;
        break;
    case 'S':
        arguments->print_scanner = true;
        break;
    case 'P':
        arguments->print_parser = true;
        break;
    case 'A':
        arguments->print_asm = true;
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

struct argp g_argp = {
    .options = g_argp_opts,
    .parser = &arg_parser,
    .args_doc = "FILE",
    .doc =
"The RockLisp compiler. Compiles a single .rl file to an asm file that can be "
"assembled by GNU `as'. \v"
"Passing in a file without -o will create an assembly file using the basename, "
"with the .s extension appended to it. "
"\nExample: chisel test.rl => test.s",
};
    
int main(int argc, char *argv[]) {
    // PARSE CMD-LINE ARGUMENTS
    struct arguments arguments = {
        .print_scanner = false,
        .print_parser = false,
        .print_asm = false,
        .ifilename = NULL,
        .ofilename = NULL,
    };

    argp_parse(&g_argp, argc, argv, 0, NULL, &arguments);

    // create output filename buffer in case -o wasn't specified.
    char *base_end = strrchr(arguments.ifilename, '.');
    size_t base_end_idx;
    if (base_end == NULL)
        base_end_idx = strlen(arguments.ifilename);
    else
        base_end_idx = base_end - arguments.ifilename;

    const char extension[] = ".s";
    char ofilename[base_end_idx + sizeof(extension)];

    if (arguments.ofilename == NULL) {
        // create output filename
        memcpy(ofilename, arguments.ifilename, base_end_idx);
        memcpy(ofilename + base_end_idx, extension, sizeof(extension));

        arguments.ofilename = ofilename;
    }

    // OPEN/READ INPUT FILE
    FILE *ifile = fopen(arguments.ifilename, "r");
    if (ifile == NULL) {
        printf("ERROR: couldn't open %s\n", arguments.ifilename);
        goto input_file_failed;
    }

    size_t capacity = 100;
    size_t length = 0;
    char *file_buffer = malloc(sizeof(char) * capacity);
    if (file_buffer == NULL) {
        printf("ERROR: couldn't create buffer space for file\n");
        goto file_buffer_failed;
    }

    size_t read_len = capacity - length;
    memset(file_buffer + length, 0, read_len);
    while (fread(file_buffer+length, sizeof(char), read_len, ifile) == read_len) {
        capacity *= 2;
        char *tmp = realloc(file_buffer, capacity * sizeof(char));
        if (tmp == NULL) {
            printf("ERROR: cannot allocate %zu bytes to load %s\n",
                   capacity, arguments.ifilename);
            free(file_buffer);
            goto file_buffer_failed;
        }
        file_buffer = tmp;

        length += read_len;
        read_len = capacity - length;

        memset(file_buffer + length, 0, read_len);
    }

    // SCAN & TOKENIZE FILE
    char *token_buffer = NULL;
    size_t num_tokens = 0;
    
    const char *scan_error =
        tokenize_buffer(file_buffer, &token_buffer, &num_tokens);
    
    if (scan_error != NULL || token_buffer == NULL) {
        printf("SCAN ERROR: %s\n", scan_error);
        goto scan_failed;
    }

    if (arguments.print_scanner) {
        printf("FILE:\n%s\nEND-FILE\n\n", file_buffer);
        printf("--SCANNER OUTPUT: %zu tokens--\n", num_tokens);

        for (size_t t = 0; t < num_tokens; t++) {
            printf("'%s' ", nth_token(token_buffer, t));

            // newline every 15 tokens.
            if ((t+1)%15 == 0)
                printf("\n");
        }
        printf("\n--END SCANNER OUPTUT--\n");
    }

    // PARSE TOKENS, GENERATE AST

    // COMPILE AST

    // OPEN OUTPUT FILE
    FILE *ofile = fopen(arguments.ofilename, "w");
    if (ofile == NULL) {
        printf("ERROR: couldn't open %s\n", arguments.ofilename);
        goto output_file_failed;
    }

    printf("writing %s\n", arguments.ofilename);

    // WRITE RESULTS

    // EXIT

    fclose(ofile);
 output_file_failed:
    // compile_ast_failed:
    // parse_tokens_failed:
    
    if (token_buffer != NULL)
        free(token_buffer);
    
 scan_failed:
    free(file_buffer);
    
 file_buffer_failed:
    fclose(ifile);
    
 input_file_failed:
    return 0;
}
