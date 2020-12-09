/**
 * IFJ Project 2020: Team 008, variant II
 * 
 * @file expression.c
 * 
 * @brief Syntax analysis for expressions
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
#include "generator.h"

#define TABLE_SIZE 8

tStack stack;

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

/**
 * @brief Cleanup function that frees stack
 * @param parser parser structure
 * @param error returned error from expression
 * @return return error parameter
 */
int cleanup(Parser parser, int error) {
	stackDispose(&stack);
	parser->tokenProcessed = false;
	return error;
}

/**
 * @brief Covnert symbol to index of precdence table
 * @param symbol symbol to be converted
 * @return Function returns index of precedence table
 */
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

/**
 * @brief Covnert token to index of precdence table
 * @param parser structure of parser where token is stored
 * @return Function returns index of precedence table
 */
int getIndexFromToken(Parser parser) {
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

/**
 * @brief Covnert token to symbol
 * @param parser structure of parser where token is stored
 * @return Function returns converted symbol
 */
int getSymbolFromToken(Parser parser) {	
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

/**
 * @brief Check if expression can be reduced using rules
 * @param count How many characters to be reduced
 * @param parser Structure of parser
 * @return Function return ERROR_CODE_OK if rule exists or error code depending on what failed. 
 */
int checkRule(int count, Parser parser){
	if (count == 1) {
		if (stack.top->data == ID || stack.top->data == INT || stack.top->data == FLOAT || stack.top->data == STRING) {
			return ERROR_CODE_OK;
		}
	}else if (count == 3) {
		if (stack.top->data == NON_TERM && stack.top->next->next->data == NON_TERM) {
			if (stack.top->next->data >= PLUS && stack.top->next->data <= DIVIDE) {
				generate_operation(stack.top->next->data, parser->exprType);
				return ERROR_CODE_OK;
			}else if (stack.top->next->data >= LESSER && stack.top->next->data <= NOT_EQUAL){
				if(parser->exprIsBool)
					return ERROR_SEM_COMP;
				parser->exprIsBool = true;
				generate_operation(stack.top->next->data, parser->exprType);
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

/**
 * @brief Reduce expression on stack to NON_TERM
 * @param parser Structure of parser
 * @return Function return error code depending if there was some error or everything passed ok.
 */
int reduce(Parser parser) {
	tItemPtr *tmp = stackTop(&stack);;
	int count = 0;
	int returnValue = 0;
	while (tmp != NULL) {
		if (tmp->data != HANDLE){
			count++;
		}else break;
		tmp = tmp->next;
	}
	returnValue = checkRule(count, parser);
	if (returnValue)
	{
		return returnValue;
	}
	for (int i = 0; i < count+1; i++) {
		stackPop(&stack);
	}
	if (stackPush(&stack, NON_TERM)) {
		return ERROR_INTERNAL;
	}

	return ERROR_CODE_OK;
}


/**
 * @brief Checks if all types are the same, basic division of zero and other semantic things.
 * @param firstToken If token is first token
 * @param divide If previous token was divide
 * @param parser Structure of parser
 * @return Function return error code
 */
int checkSemantic(bool firstToken, bool *divide, Parser parser)
{
	if (isType(TOKEN_INT))
	{
		if (parser->exprType == tNONE)
		{
			parser->exprType = tINT;
		}
		else if (parser->exprType != tINT)
			return ERROR_SEM_COMP;
	}
	else if (isType(TOKEN_FLOAT))
	{
		if (parser->exprType == tNONE)
			parser->exprType = tFLOAT64;
		else if (parser->exprType != tFLOAT64)
		{
			return ERROR_SEM_COMP;
		}
	}
	else if (isType(TOKEN_STRING))
	{
		if (parser->exprType == tNONE)
			parser->exprType = tSTRING;
		else if (parser->exprType != tSTRING)
			return ERROR_SEM_COMP;
	}
	else if (isType(TOKEN_IDENTIFIER))
	{
		if (!strCmpConstStr(parser->token.attribute.string, "_"))
			return ERROR_SEM;
		parser->currentID = symtableReadStack(&parser->sLocal, parser->token.attribute.string->str);
		if (parser->currentID == NULL)
		{
			if (firstToken)
			{
				parser->funcInExpr = true;
				return ERROR_CODE_OK;
			}
			else
				return ERROR_SEM;
		}
		else
		{
			if (parser->exprType == tNONE)
				parser->exprType = parser->currentID->type;
			else if (parser->exprType != parser->currentID->type)
				return ERROR_SEM_COMP;
		}
	}
	if (parser->token.type >= TOKEN_EQ && parser->token.type <= TOKEN_GT)
	{
		if (!parser->exprBoolAllowed)
			return ERROR_SEM_COMP;
	}
	if (parser->exprType == tSTRING && parser->token.type >= TOKEN_MINUS && parser->token.type <= TOKEN_DIV)
		return ERROR_SEM_COMP;
	if(isType(TOKEN_DIV))
		*divide = true;
	else if(!isType(TOKEN_INT))
		*divide = false;
	if(*divide && isType(TOKEN_INT))
	{
		if(parser->token.attribute.integer == 0)
			return ERROR_DIV_ZERO;
		else
			*divide = false;
	}
	return ERROR_CODE_OK;
}

/**
 * @brief Syntax and semantic analysis for expressions
 * @param parser Structure of parser
 * @return Function return error code
 */
int expression(Parser  parser) {
	stackInit(&stack);
	char index;
	int end = 0;
	bool firstToken = true;
	int returnCode = 0;
	bool idFirst = false;
	bool divide = false;
	parser->funcInExpr = false;
	parser->exprType = tNONE;
	parser->exprIsBool = false;
	int semanticCode = 0;

	if (stackPush(&stack, DOLAR)) {
		return cleanup(parser, ERROR_INTERNAL);
	}

	tItemPtr *top_terminal;
	Prec_symbol symbol;
	getToken();

	while(!end) {
		if(!semanticCode)
		{
			semanticCode = checkSemantic(firstToken, &divide, parser);
			if(parser->funcInExpr)
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
				generate_code(push);
				if (stackInsertAfterTerm(&stack, HANDLE)) {
					return cleanup(parser, ERROR_INTERNAL);
				}
				if (stackPush(&stack, symbol)) {
					return cleanup(parser, ERROR_INTERNAL);
				}
				getToken();
				break;
			case '>':
				returnCode = reduce(parser);
				if(returnCode) {
    				return cleanup(parser, returnCode);
				}
				break;
			default:
				if (top_terminal->data != DOLAR && getSymbolFromToken(parser) == DOLAR){
					end = 1;
					break;
				}else return cleanup(parser, ERROR_SYN);
		}
		firstToken = false;
	}

	if (!(stack.top->data == NON_TERM && stack.top->next->data == DOLAR)) {
		return cleanup(parser, ERROR_SYN);
	}

	if(semanticCode)
		return cleanup(parser, semanticCode);

	if(parser->exprBoolAllowed && !parser->exprIsBool)
		return cleanup(parser, ERROR_SEM_COMP);

	return cleanup(parser, ERROR_CODE_OK);
}
