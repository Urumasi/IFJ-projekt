/**
 * IFJ Project 2020: Team 008, variant II
 *
 * @file generator.c
 *
 * @brief Code generator implementation.
 *
 *
 * @author Martin Kneslík <xknesl02@stud.fit.vutbr.cz>
 * @author Adriana Jurkechová <xjurke02@stud.fit.vutbr.cz>
 * @author Karel Norek <xnorek01@stud.fit.vutbr.cz>
 * @author Petr Salaba <xsalab00@stud.fit.vutbr.cz>
 */

#include "generator.h"

// parser generators

int generate_push(Token *token) {
	switch (token->type) {
		case TOKEN_INT:
			printf("PUSHS int@%d\n", token->attribute.integer);
			break;
		case TOKEN_FLOAT:
			printf("PUSHS int@%a\n", token->attribute.decimal);
			break;
		case TOKEN_STRING:
			printf("PUSHS string@%s\n", token->attribute.string->str);
			break;
		case TOKEN_IDENTIFIER:
			printf("PUSHS %s\n", token->attribute.string->str);
			break;
		default:
			break;
	}
	return 0;
}

int generate_func(Token *token) {
	printf("func %s\n", token->attribute.string->str);
	return 0;
}

int generate_func_end(Token *token) {
	printf("endfunc\n");
	return 0;
}

int generate_return(Token *token) {
	printf("RETURN\n");
	return 0;
}

int generate_func_call(Token *token){
	printf("CALL ~^\n");
	return 0;
}

// expression generators

void generate_operation(Prec_symbol symbol) {
	switch (symbol) {
		case PLUS:
			printf("ADDS\n");
			break;
		case MINUS:
			printf("SUBS\n");
			break;
		case MULTIPLY:
			printf("MULS\n");
			break;
		case DIVIDE:
			printf("DIVS\n");
			break;
		default:
			printf("* OPERATION %d\n", symbol);
			break;
			//return 1;
	}
}
