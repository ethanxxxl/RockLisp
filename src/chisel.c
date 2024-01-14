#include <argp.h>
#include <stdbool.h>
#include <stdio.h>

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
"The RockLisp compiler. Compiles a single .rl file to an asm file that can be \
assembled by GNU as.",
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

    printf("ifile: %s\nofile: %s\n",
           arguments.ifilename, arguments.ofilename);
    if (arguments.print_scanner)
        printf("printing scanner...\n");
    if (arguments.print_parser)
        printf("printing parser...\n");
    if (arguments.print_asm)
        printf("printing asm...\n");

    // OPEN INPUT FILE

    // SCAN & TOKENIZE FILE

    // PARSE TOKENS, GENERATE AST

    // COMPILE AST

    // OPEN OUTPUT FILE

    // WRITE RESULTS

    // EXIT
    
    return 0;
}
