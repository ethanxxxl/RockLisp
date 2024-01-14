#include "scanner.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

const char g_delims[] = "[](){}.'";

const char* tokenize_buffer(const char *code,
                            struct Tokens *tokens) {
    char *error = NULL;
    size_t code_len = strlen(code);

    // the maximum length needed to tokenize `code' is 2*len(code), ie every
    // character is a token, and needs a \0;
    tokens->buffer = malloc(code_len * sizeof(char) * 2);
    if (tokens->buffer == NULL)
        return "couldn't allocate space for token buffer";

    size_t *num_tokens = &tokens->num;
    *num_tokens = 0;
    char *token_pos = tokens->buffer;
    const char *c = code;
    while (*c != '\0') {
        // Skip any leading whitespace
        while (isspace(*c) || *c == ';') {
            if (*c == ';')
                c = strchr(c, '\n');
            else
                c++;

            if (c == NULL || *c == '\0')
                return NULL;
        }
        
        // Token is a DELIMETER
        if (strchr(g_delims, *c) != NULL) {
            *token_pos++ = *c;
            *token_pos++ = '\0';
            (*num_tokens)++;
            c++;
            continue;
        }

        // Token is a STRING
        if (*c == '"') {
            const char *tok_end = strchr(c+1, '"') + 1;
            if (tok_end == NULL) {
                error =  "unmatched quotation mark";
                goto return_error;
            }

            // add the string to the token list
            memcpy(token_pos, c, tok_end - c);
            token_pos += tok_end - c;
            *token_pos++ = '\0';
            (*num_tokens)++;
            c = tok_end;
            continue;
        }

        // Token is a SYMBOL or NUMBER
        const char *tok_beg = c;
        while (strchr(g_delims, *c) == NULL
               && !isspace(*c)
               && *c != ';'
               && *c != '"'
               && *c != '\0') c++;

        memcpy(token_pos, tok_beg, c - tok_beg);
        token_pos += c - tok_beg;
        *token_pos++ = '\0';
        *num_tokens += 1;
    }       

    // fill out the tokens array.
    tokens->tokens = malloc(tokens->num * sizeof(char*));
    if (tokens->tokens == NULL) {
        error = "couldn't allocate space for token buffer";
        goto return_error;
    }

    char *tok = tokens->buffer;
    for (size_t i = 0; i < tokens->num; i++) {
        tokens->tokens[i] = tok;
        tok = strchr(tok, '\0')+1;
    }

    return NULL;

 return_error:
    free(tokens->tokens);
    return error;
}

const char* nth_token(struct Tokens tokens, size_t n) {
    return tokens.tokens[n];
}
