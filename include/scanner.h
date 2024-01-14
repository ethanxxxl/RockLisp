#ifndef SCANNER_H
#define SCANNER_H

#include <stddef.h>

extern const char g_tokens[];

/**
 * Mallocs a buffer in `tokens', and tokenizes `code'. The number of tokens
 * found are recorded in `num_tokens'
 *
 * @param[in] code a buffer holding code to tokenize
 * @param[out] tokens the tokenized version of `code'
 * @param[out] num_tokens the number of tokens in `tokens'
 *
 * @return NULL on success, or an error message.
 * */
const char* tokenize_buffer(const char *code, char **tokens, size_t *num_tokens);

// returns the nth token in a tokenized string.
// WARNING, this implements an inneficient algorithm.
const char* nth_token(const char *tokens, size_t n);

#endif
