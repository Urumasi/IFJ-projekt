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

#include "scanner.h"
#include "expression.h"
#include "str.h"

#define generate_code(code_type) \
    if (generate_ ## code_type (&parser->token)) \
    return ERROR_INTERNAL

typedef enum {
	NONE,
	ADD,
	SUB,
	MUL,
	DIV,
	ASSIGN,
	DEFINE,
	JUMP,
	CALL,
	RETURN,
	LABEL,
} InstructionType;

typedef struct s_TAC { // Three address code
	InstructionType type;
	string destination;
	string operand1;
	string operand2;
} TAC;

// parser generators

int generate_push(Token *token);

int generate_func(Token *token);

int generate_func_end(Token *token);

int generate_return(Token *token);

int generate_func_call(Token *token);

// expression generators

void generate_operation(Prec_symbol symbol);

/*
void generate_prog_init();

int generate_if_then();

void generate_if_else(unsigned int label);

void generate_if_end(unsigned int label);

void generate_var_definition(Token *token);

void generate_var_definition_assign(string *var_name);

void generate_comparison(Prec_symbol symbol);

*/

#endif //__GENERATOR_H__
