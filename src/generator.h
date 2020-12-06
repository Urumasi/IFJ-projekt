/**
 * IFJ Project 2020: Team 008, variant II
 *
 * @file generator.h
 *
 * @brief Code generator.
 *
 *
 * @author Martin Kneslík <xknesl02@stud.fit.vutbr.cz>
 * @author Adriana Jurkechová <xjurke02@stud.fit.vutbr.cz>
 * @author Karel Norek <xnorek01@stud.fit.vutbr.cz>
 * @author Petr Salaba <xsalab00@stud.fit.vutbr.cz>
 */

#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include <stdbool.h>
#include "parser.h"
#include "expression.h"
#include "str.h"
#include "taclist.h"

#define generate_code(code_type) \
    if (generate_ ## code_type (parser)) \
    return ERROR_INTERNAL;

// Symbol stack

typedef struct s_sstack_item {
    struct s_sstack_item *next;
    struct s_sstack_item *prev;
    TAC_addr *data;
} *sstack_item_ptr;

typedef struct s_sstack {
    sstack_item_ptr head;
    sstack_item_ptr tail;
    unsigned int length;
} sstack;

int ss_init(sstack *stack);
void ss_free(sstack *stack);
int ss_push(sstack *stack, TAC_addr *data);
int ss_push_copy(sstack *stack, TAC_addr data);
TAC_addr *ss_pop(sstack *stack);
bool ss_empty(sstack *stack);

// Base function

void print_taclist();

void generate_end();

// Parser generators

int generate_prog_init(Parser parser);

int generate_push(Parser parser);

int generate_func(Parser parser);

int generate_func_end(Parser parser);

int generate_return(Parser parser);

int generate_func_call(Parser parser);

int generate_define_var(Parser parser);

int generate_assign_push_id(Parser parser);

int generate_assign(Parser parser);

// Expression generators

int generate_operation(Prec_symbol symbol);

#endif //__GENERATOR_H__
