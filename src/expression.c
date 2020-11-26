/**
 * IFJ Project 2020: Team 008, variant II
 * 
 * @file expression.c
 * 
 * @brief
 *
 * 
 * @author Martin Kneslík <xknesl02@stud.fit.vutbr.cz>
 * @author Adriana Jurkechová <xjurke02@stud.fit.vutbr.cz>
 * @author Karel Norek <xnorek01@stud.fit.vutbr.cz>
 * @author Petr Salaba <xsalab00@stud.fit.vutbr.cz>
 */

#include <stdio.h>
#include <stdbool.h>
#include "scanner.h"
#include "parser.h"
#include "expression.h"
#include "stack.h"
#include "error.h"

#define TABLE_SIZE 7

tStack stack;

/*
TODO
syntaktická kontrola výrazů
-nesmí být prázdný výraz
-nesmí se vyskytovat volání funkce
-volání funkce může být jen na začátku a nesmí pokračovat výrazem ( a, b = func() ) - domluvíme se potom
Příklad:
    x := fn() + 5 - syntaktická chyba, pokud nemáš FUNEXP
    x := fn() - syntaktická chyba, pokud nemáš MULTIVAL 
    x = fn() + 5 - syntaktická chyba, pokud nemáš FUNEXP
    x = fn() - vždy syntakticky správně
sémantika - možná budu dělat já
-kontrola dělení 0
-kontrola typů
-kontrola návratové hodnoty
    - v if/for to musí vracet BOOL, takže tam musí být porovnávání
    - v přiřazení/dekleraci porovnávání být nesmí
*/

const char prec_table[TABLE_SIZE][TABLE_SIZE] = {
//	|+- |*/ | ( | ) | i | $ |
	{'>','<','<','>','<','>'},	// +-
	{'>','>','<','>','<','>'},	// */
	{'<','<','<','=','<','-'},	// (
	{'>','>','-','>','-','>'},	// )
	{'>','>','-','>','-','>'},	// i
	{'<','<','<','-','<','-'},	// $
};

int getIndexFromSymbol(Prec_symbol symbol) {
	if (symbol == PLUS || symbol == MINUS) {
		return I_PLUS_MINUS;
	}else if (symbol == MULTIPLY || symbol == DIVIDE) {
		return I_MUL_DIV;	
	}else if (symbol == LEFT_BRACKET) {
		return I_LEFT_BRACKET;
	}else if (symbol == RIGHT_BRACKET) {
		return I_RIGHT_BRACKET;
	}else if (symbol == ID || symbol == INT || symbol == FLOAT || symbol == STRING) {
		return I_ID;
	}else if (symbol == DOLAR || symbol == NON_TERM) {
		return I_DOLAR;
	}
}

int getIndexFromToken(Parser *parser) {
	if (isType(TOKEN_PLUS) || isType(TOKEN_MINUS)) {
		return I_PLUS_MINUS;
	}else if (isType(TOKEN_MUL) || isType(TOKEN_DIV)) {
		return I_MUL_DIV;
	}else if (isType(TOKEN_LBRACKET)) {
		return I_LEFT_BRACKET;
	}else if (isType(TOKEN_RBRACKET)) {
		return I_RIGHT_BRACKET;
	}else if (isType(TOKEN_IDENTIFIER) || isType(TOKEN_INT) || isType(TOKEN_FLOAT) || isType(TOKEN_STRING)) {
		return I_ID;
	}else if (isType(TOKEN_LCURLYBRACKET) || isType(TOKEN_EOL) || isType(TOKEN_SEMICOLON)) {
		return I_DOLAR;
	}else return -1;
}

int getSymbolFromToken(Parser *parser) {	
	if (isType(TOKEN_PLUS)) {
		return PLUS;
	}else if (isType(TOKEN_MINUS)) {
		return MINUS;
	}else if (isType(TOKEN_MUL)) {
		return MULTIPLY;
	}else if (isType(TOKEN_DIV)) {
		return DIVIDE;
	}else if (isType(TOKEN_LBRACKET)) {
		return LEFT_BRACKET;
	}else if (isType(TOKEN_RBRACKET)) {
		return RIGHT_BRACKET;
	}else if (isType(TOKEN_IDENTIFIER)) {
		return ID;
	}else if (isType(TOKEN_INT)) {
		return INT;
	}else if (isType(TOKEN_FLOAT)) {
		return FLOAT;
	}else if (isType(TOKEN_STRING)) {
		return STRING;
	}else return DOLAR;
}

int reduce() {
	tItemPtr *tmp;
	tmp = stackTop(&stack);
	int count = 0;
	while (tmp != NULL) {
		if (tmp->data != HANDLE){
			count++;
		}else break;
		tmp = tmp->next;
	}
	
	//rintf("------------ %d ----------\n", count);
	for (int i = 0; i < count+1; i++) {
		stackPop(&stack);
	}

	stackPush(&stack, NON_TERM);

	return ERROR_CODE_OK;
}

void printStack(){
	tItemPtr *tmp = stackTop(&stack);
	int i = 0;
	while (tmp != NULL) {
		i++;
		printf("Item: %d. Value: %d. \n", i, tmp->data);
		tmp = tmp->next;
	}
	
}

int expression(Parser  *parser) {
	stackInit(&stack);
	int ok = 0;
	if (stackPush(&stack, DOLAR)) {
		stackDispose(&stack);
		return ERROR_INTERNAL;
	}
		
	tItemPtr *top_terminal;
	Prec_symbol symbol;
	getToken();   

	do {
		symbol = getSymbolFromToken(parser);
		top_terminal = stackTop(&stack);
	
		if (top_terminal->data == NON_TERM && top_terminal->next->data != DOLAR) {
			top_terminal = top_terminal->next;
		}
		//printf("stack: %d token: %d \n", top_terminal->data, getIndexFromToken(parser));
		switch (prec_table[getIndexFromSymbol(top_terminal->data)][getIndexFromToken(parser)]) {
			case '=':
				if (stackPush(&stack, DOLAR)) {
					stackDispose(&stack);
					return ERROR_INTERNAL;
				}
				getToken();
				break;
			case '<':
				stackInsertAfterTerm(&stack, HANDLE);
				stackPush(&stack, symbol);
				getToken();
				break;
			case '>':
				reduce();
				break;
			default:
				if ((top_terminal->data != DOLAR && getSymbolFromToken(parser) == DOLAR)){
					ok = 1;
				}
				break;
		}
		
		printStack();
		printf("--------------------------\n");
		//printf("\nSYMBOL %d\n", getSymbolFromToken(parser));
    } while (!ok);
    
	
	parser->tokenProcessed = false;

	stackDispose(&stack);
    return ERROR_CODE_OK;
}