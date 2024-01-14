#include "scanner.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

const char g_tokens[] = "[]{}.\"";

const char* tokenize_buffer(const char *code,
                            char **tokens, size_t *num_tokens) {
    size_t code_len = strlen(code);

    // the maximum length needed to tokenize `code' is 2*len(code), ie every
    // character is a token, and needs a \0;
    *tokens = malloc(code_len * sizeof(char) * 2);
    if (*tokens == NULL)
        return "couldn't allocate space for token buffer";

    *num_tokens = 0;
    char *token_pos = *tokens;
    for (const char *c = code; *c != '\0'; c++) {
        if (strchr(g_tokens, *c) != NULL) {
            if (c > code
                && strchr(g_tokens, *(c-1)) == NULL
                && !isspace(*(c-1))) {
                // the last token needed to be finished off
                *token_pos++ = '\0';
                (*num_tokens)++;
            }
                
            *token_pos++ = *c;
            *token_pos++ = '\0';
            
            (*num_tokens)++;
            continue;
        }
        
        // whitespace will end the previous token and start a new one.
        // we skip contiguous blocks of whitespace/comments
        if (isspace(*c) || *c == ';') {
            if (*num_tokens > 0 && strchr(g_tokens, *(c-1)) == NULL) {
                // finish last token/start new one.
                *token_pos++ = '\0';
                (*num_tokens)++;
            } else {
                // don't create a new token. either:
                //   - these are leading comments
                //   - the character before this was a token delimiter
            }

            do {
                // skip any additional whitespace.
                if (*c == ';')
                    c = strchr(c, '\n');
                else
                    c++;

                // if it was a comment, it could be the last line in the file.
                if (c == NULL)
                    break;

            } while (isspace(*c) || *c == ';');
            c--; // c will be incremented next iteration.
            continue;
        }

        *token_pos++ = *c;
    }

    return NULL;
}

const char* nth_token(const char *tokens, size_t n) {
    // XXX consider caching the token locations to make this function faster.
    
    const char *token = tokens;
    for (size_t tok = 0; tok < n; tok++) {
        token = strchr(token, '\0')+1;
    }

    return token;
}
