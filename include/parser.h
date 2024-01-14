#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include <stdbool.h>

struct Car;
struct Cons;
struct Symbol;

enum Types {
    SYMBOL,
    CONS,
    STR,
    INTEGER,
    FLOAT,
    POINTER,
};

struct Symbol {
    const char* name;
    void* binding;
};

union Value {
    struct Symbol symbol;
    struct Cons   *cons;

    const char *str;
    int i32;
    float f32;
    void* void_p;
};

struct Car {
    enum Types type;
    
    union Value value;
};

struct Cons {
    struct Car car;
    struct Cons *cdr;
};


struct Cons* cons_find(struct Cons* cons, struct Car car);

void print_car(struct Car car);
void print_cons(struct Cons *cons);
struct Cons* make_cons(void);
void free_cons(struct Cons *cons);
bool is_nil(struct Cons *cons);

/**

 */
struct Cons* parse_tokens(struct Tokens tokens);

#endif
