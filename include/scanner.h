#ifndef SCANNER_H
#define SCANNER_H

#include <stddef.h>

extern const char g_tokens[];

struct Tokens {
    const char **tokens;
    char *buffer;
    size_t num;
};

/**
 * Initializes `tokens' and then tokenizes `code', recording the results in
 * `tokens'
 *
 * @param[in] code a buffer holding code to tokenize
 * @param[out] tokens structure for keeping track of tokens in code.
 *
 * @return NULL on success, or an error message.
 **/
const char* tokenize_buffer(const char *code, struct Tokens *tokens);

// returns the nth token in a tokenized string.
// WARNING, this implements an inneficient algorithm.
const char* nth_token(struct Tokens tokens, size_t n);

#endif
