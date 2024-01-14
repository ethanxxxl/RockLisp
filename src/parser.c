#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>

#include "parser.h"
#include "scanner.h"

struct Cons* parse_next_sexp(struct Tokens *tokens, size_t *index,
                             struct Cons *symbols) {
    struct Cons fake_head;
    struct Cons *head = &fake_head;
    struct Cons *tail = head;

    for (size_t *t = index; *t < tokens->num; *t += 1) {
        const char *token = nth_token(*tokens, *t);

        // CONS type
        if (*token == '[') {
            *t += 1;
            tail->cdr = make_cons();
            tail = tail->cdr;
                
            tail->car.type = CONS;
            tail->car.value.cons = parse_next_sexp(tokens, t, symbols);

            if (tail->car.value.cons == NULL) {
                // TODO an error occured
                free_cons(head->cdr);
                return NULL;
            }

            continue;
        }

        if (*token == ']') {
            tail->cdr = make_cons();
            return head->cdr;
        }

        // STR type
        if (*token == '"') {
            tail->cdr = make_cons();
            tail = tail->cdr;

            tail->car = (struct Car){
                .type = STR,
                .value.str = token,
            };
            continue;
        }

        // INTEGER type
        if (isdigit(*token)) {
            errno = 0;
            char *tailptr = NULL;
            int num = strtol(token, &tailptr, 0);
            if (tailptr == token
                || tailptr == token + strlen(token)
                || errno != 0) {
                // syntax error
                free_cons(head->cdr);
                return 0;
            }

            tail->cdr = make_cons();
            tail = tail->cdr;

            tail->car = (struct Car){
                .type = INTEGER,
                .value.i32 = num
            };
            continue;
        }

        // create a new symbol (if necessary)
        struct Symbol new_symbol = (struct Symbol){
            .name = token,
            .binding = NULL,
        };
        
        struct Cons *symbol =
            cons_find(symbols, (struct Car){ .type = SYMBOL,
                                             .value.symbol = new_symbol });

        // symbol wasn't in the list already, so add it.
        if (is_nil(symbol)) {
            symbol->car =
                (struct Car){.type = SYMBOL, .value.symbol = new_symbol};

            symbol->cdr = make_cons(); // add new nil to end of list
        }

        // SYMBOL type
        tail->cdr = make_cons();
        tail = tail->cdr;

        // todo this method of storeing symbols might need to change to
        // accomodate later stages in the compiler.
        tail->car = (struct Car) {
            .type = SYMBOL,
            .value.symbol = new_symbol
        };
    }

    tail->cdr = make_cons(); // nil terminate list
    return head->cdr;
}


struct Cons* parse_tokens(struct Tokens tokens) {
    size_t index = 0;
    struct Cons *symbols = make_cons();
    struct Cons *sexp = parse_next_sexp(&tokens, &index, symbols);

    free_cons(symbols);

    return sexp;
}


struct Cons* make_cons(void) {
    struct Cons *cons = malloc(sizeof(struct Cons));
    if (cons == NULL)
        return NULL;

    cons->cdr = cons;
    cons->car = (struct Car){
        .type = CONS,
        .value.cons = cons,
    };

    return cons;
}

bool car_eq(struct Car car1, struct Car car2) {
    if (car1.type != car2.type)
        return false;

    switch (car1.type) {
    case SYMBOL:
        return strcmp(car1.value.symbol.name, car2.value.symbol.name) == 0;
    case CONS:
        return car1.value.cons == car2.value.cons;
    case STR:
        return strcmp(car1.value.str, car2.value.str) == 0;
    case INTEGER:
        return car1.value.i32 == car2.value.i32;
    case FLOAT:
        return car1.value.f32 == car2.value.f32;
    case POINTER:
        return car1.value.void_p == car2.value.void_p;
    }
}

struct Cons* cons_find(struct Cons *cons, struct Car car) {
    struct Cons *tst = cons;
    for (; !is_nil(tst); tst = tst->cdr) {
        if (car_eq(car, tst->car))
            return tst; // found a match.
    }

    // tst is nill
    return tst;
}

void free_cons(struct Cons *cons) {
    if (cons == NULL)
        return;
    
    if (cons == cons->cdr) {
        free(cons);
        return;
    }

    if (cons->car.type == CONS)
        free_cons(cons->car.value.cons);

    if (cons->cdr != NULL)
        free_cons(cons->cdr);

    free(cons);
}

bool is_nil(struct Cons *cons) {
    if (cons == NULL ||
        cons->cdr == NULL)
        return false;

    if (cons->car.type == CONS &&
        cons->car.value.cons == cons &&
        cons->cdr == cons)
        return true;

    return false;
}

void print_car(struct Car car) {
    switch (car.type) {
    case SYMBOL:
        printf("%s", car.value.symbol.name);
        break;
    case CONS:
        print_cons(car.value.cons);
        break;
    case STR:
        printf("%s", car.value.str);
        break;
    case INTEGER:
        printf("%d", car.value.i32);
        break;
    case FLOAT:
        printf("%f", car.value.f32);
        break;
    case POINTER:
        printf("%p", car.value.void_p);
        break;
    }
}

void print_cons(struct Cons *cons) {
    if (cons == NULL) {
        printf("(NULL)");
        return;
    }
    
    printf("[");

    for (struct Cons *c = cons; !is_nil(c); c = c->cdr) {
        if (c->cdr == NULL || c->cdr == c) {
            printf(". ");
            print_car(c->car);
            break;
        } else if (is_nil(c->cdr)) {
            print_car(c->car);
            break;
        } else {
            print_car(c->car);
            printf(" ");
        }
    }

    printf("]");
}
