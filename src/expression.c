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
#include "expression.h"
#include "stack.h"
#include "error.h"
#include "generator.h"

#define TABLE_SIZE 8

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
//	|+- |*/ | ( | ) | i | r | $ |
	{'>','<','<','>','<','>','>'},	// +-
	{'>','>','<','>','<','>','>'},	// */
	{'<','<','<','=','<','<','-'},	// (
	{'>','>','-','>','-','>','>'},	// )
	{'>','>','-','>','-','>','>'},	// i
	{'<','<','<','>','<','-','>'},	// r
	{'<','<','<','-','<','<','-'},	// $
};

int cleanup(Parser *parser, int error) {
	stackDispose(&stack);
	parser->tokenProcessed = false;
	return error;
}

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
	}else if (symbol == LESSER || symbol == LESS_OR_EQ || symbol == GREATER || symbol == GRT_OR_EQ || symbol == EQUAL || symbol == NOT_EQUAL){
		return I_RELATION;
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
	}else if (isType(TOKEN_LT) || isType(TOKEN_LEQ) || isType(TOKEN_GT) || isType(TOKEN_GEQ) || isType(TOKEN_EQ) || isType(TOKEN_NEQ)){
		return I_RELATION;
	}else return I_DOLAR;
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
	}else if (isType(TOKEN_LT)) {
		return LESSER;
	}else if (isType(TOKEN_LEQ)) {
		return LESS_OR_EQ;
	}else if (isType(TOKEN_GT)) {
		return GREATER;
	}else if (isType(TOKEN_GEQ)) {
		return GRT_OR_EQ;
	}else if (isType(TOKEN_EQ)) {
		return EQUAL;
	}else if (isType(TOKEN_NEQ)) {
		return NOT_EQUAL;
	}else return DOLAR;
}

int checkRule(int count){
	if (count == 1) {
		if (stack.top->data == ID || stack.top->data == INT || stack.top->data == FLOAT || stack.top->data == STRING) {
			return ERROR_CODE_OK;
		}
	}else if (count == 3) {
		if (stack.top->data == NON_TERM && stack.top->next->next->data == NON_TERM) {
			if (stack.top->next->data == PLUS || stack.top->next->data == MINUS || stack.top->next->data == MULTIPLY ||stack.top->next->data == DIVIDE) {
                generate_binary_op(stack.top->next->data);
				return ERROR_CODE_OK;
			}else if (stack.top->next->data == LESSER || stack.top->next->data == LESS_OR_EQ || stack.top->next->data == GRT_OR_EQ ||stack.top->next->data == GREATER){
			    generate_comparison(stack.top->next->data);
				return ERROR_CODE_OK;
            }else if (stack.top->next->data == EQUAL || stack.top->next->data == NOT_EQUAL){
                generate_comparison(stack.top->next->data);
                return ERROR_CODE_OK;
			}
		}else if (stack.top->data == RIGHT_BRACKET && stack.top->next->next->data == LEFT_BRACKET){
			if (stack.top->next->data == NON_TERM){
				return ERROR_CODE_OK;
			}
		}
	}
	return ERROR_SYN;
}

void printStack(){
	tItemPtr *tmp = stackTop(&stack);
	int i = 0;
	while (tmp != NULL) {
		i++;
		printf("Item: %d. Value: %d. \n", i, tmp->data);
		tmp = tmp->next;
	}
	printf("--------------------------\n");
}

int reduce() {
	tItemPtr *tmp = stackTop(&stack);
	int count = 0;
	while (tmp != NULL) {
		if (tmp->data != HANDLE){
			count++;
		}else break;
		tmp = tmp->next;
	}
		
	if (checkRule(count)){
		return ERROR_SYN;
	}
	for (int i = 0; i < count+1; i++) {
		stackPop(&stack);
	}
	if (stackPush(&stack, NON_TERM)) {
		return ERROR_INTERNAL;
	}

	return ERROR_CODE_OK;
}

int expression(Parser  *parser) {
	stackInit(&stack);
	char index;
	int end = 0;
	int tokenCount = 0;
	int returnCode = 0;
	bool idFirst = false;

	if (stackPush(&stack, DOLAR)) {
		return cleanup(parser, ERROR_INTERNAL);
	}
		
	tItemPtr *top_terminal;
	Prec_symbol symbol;
	getToken(); 

	while(!end) {
		tokenCount++;
		if (tokenCount == 1 && isType(TOKEN_IDENTIFIER)) {
			idFirst = true;
		}else if (tokenCount == 2 && idFirst && isType(TOKEN_LBRACKET)) {
			parser->funcInExpr = true;
			return cleanup(parser, ERROR_CODE_OK);
		}

		symbol = getSymbolFromToken(parser);
		top_terminal = stackTop(&stack);
		if (top_terminal->data == NON_TERM && top_terminal->next->data != DOLAR) {
			top_terminal = top_terminal->next;
		}
		index = prec_table[getIndexFromSymbol(top_terminal->data)][getIndexFromToken(parser)];
		switch (index) {
			case '=':
				if (stackPush(&stack, RIGHT_BRACKET)) {
					return cleanup(parser, ERROR_INTERNAL);
				}
				getToken();
				break;
			case '<':
			    generate_push(&parser->token);
				if (stackInsertAfterTerm(&stack, HANDLE)) {
					return cleanup(parser, ERROR_INTERNAL);
				}
				if (stackPush(&stack, symbol)) {
					return cleanup(parser, ERROR_INTERNAL);
				}
				getToken();
				break;
			case '>':
				returnCode = reduce();
				if(returnCode == ERROR_SYN) {
    				return cleanup(parser, ERROR_SYN);
				}else if (returnCode == ERROR_INTERNAL) {
					return cleanup(parser, ERROR_INTERNAL);
				}
				break;
			default:
				if (top_terminal->data != DOLAR && getSymbolFromToken(parser) == DOLAR){
					end = 1;
					break;
				}else return cleanup(parser, ERROR_SYN);
		}
		//printStack();
    }

	if (!(stack.top->data == NON_TERM && stack.top->next->data == DOLAR)) {
		return cleanup(parser, ERROR_SYN);
	}

    return cleanup(parser, ERROR_CODE_OK);
}