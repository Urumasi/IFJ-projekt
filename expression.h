/**
 * IFJ Project 2020: Team 008, variant II
 * 
 * @file expression.h
 * 
 * @brief Header file for expression.h
 *
 * 
 * @author Martin Kneslík <xknesl02@stud.fit.vutbr.cz>
 * @author Adriana Jurkechová <xjurke02@stud.fit.vutbr.cz>
 * @author Karel Norek <xnorek01@stud.fit.vutbr.cz>
 * @author Petr Salaba <xsalab00@stud.fit.vutbr.cz>
 */

#ifndef __EXPRESSION_H__
#define __EXPRESSION_H__

#include "parser.h"

typedef enum {
	PLUS,
	MINUS,
	MULTIPLY,
	DIVIDE,

	LEFT_BRACKET,
	RIGHT_BRACKET,

	LESSER,
	LESS_OR_EQ,
	GREATER,
	GRT_OR_EQ,
	EQUAL,
	NOT_EQUAL,

	ID,
	INT,
	FLOAT,
	STRING,

	DOLAR,
	HANDLE,
	NON_TERM

}Prec_symbol;

typedef enum {
    I_PLUS_MINUS,
    I_MUL_DIV,

    I_LEFT_BRACKET,
    I_RIGHT_BRACKET,
    
    I_ID,

	I_RELATION,
 
    I_DOLAR

}Prec_index;

int cleanup(Parser parser, int error);
int getIndexFromSymbol(Prec_symbol symbol);
int getIndexFromToken(Parser parser);
int getSymbolFromToken(Parser parser);
int checkRule(int count, Parser parser);
int reduce(Parser parser);
int checkSemantic(bool firstToken, bool *divide, Parser parser);
int expression(Parser parser);

#endif //__EXPRESSION_H__
