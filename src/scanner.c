/**
 * IFJ Project 2020: Team 008, variant II
 *
 * @file scanner.c
 *
 * @brief
 *
 *
 * @author Martin Kneslík <xknesl02@stud.fit.vutbr.cz>
 * @author Adriana Jurkechová <xjurke02@stud.fit.vutbr.cz>
 * @author Karel Norek <xnorek01@stud.fit.vutbr.cz>
 * @author Petr Salaba <xsalab00@stud.fit.vutbr.cz>
 */

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

#include "scanner.h"
#include "error.h"


#define SCANNER_STATE_START 1
#define SCANNER_STATE_EOL 2
#define SCANNER_STATE_KEYWORD_OR_IDENTIFIER 3
#define SCANNER_STATE_NUMBER 4
#define SCANNER_STATE_DECIMAL_POINT 5
#define SCANNER_STATE_FLOAT 6
#define SCANNER_STATE_EXPONENT 7
#define SCANNER_STATE_EXPONENT_SIGNED 8
#define SCANNER_STATE_FINAL_NUMBER 9 
#define SCANNER_STATE_STRING_START 10
#define SCANNER_STATE_STRING 11 
#define SCANNER_STATE_ESCAPE 12
#define SCANNER_STATE_LESS_THAN 13
#define SCANNER_STATE_MORE_THAN 14
#define SCANNER_STATE_COMMENTARY 15
#define SCANNER_STATE_BLOCK_COMMENTARY_START 16
#define SCANNER_STATE_BLOCK_COMMENTARY_EXIT 17
#define SCANNER_STATE_ASSIGN 18
#define SCANNER_STATE_EQUAL 19
#define SCANNER_STATE_NOT_EQUAL 20




FILE* sourceFile;
string* setStr;

void setSourceFile(FILE* f) {
    sourceFile = f;
}

void setString(string* str) {
    setStr = str;
}

static int freeResources(int exitCode, string* str) {
    strFree(str);
    return exitCode;
}


static int processingKeywordIdentifier(string* str, Token* token) {
	if (!strCmpConstStr(str, "else")) token->attribute.keyword = KW_ELSE;
	else if (!strCmpConstStr(str, "float64")) token->attribute.keyword = KW_FLOAT64;
	else if (!strCmpConstStr(str, "for")) token->attribute.keyword = KW_FOR;
	else if (!strCmpConstStr(str, "func")) token->attribute.keyword = KW_FUNC;
	else if (!strCmpConstStr(str, "if")) token->attribute.keyword = KW_IF;
	else if (!strCmpConstStr(str, "int")) token->attribute.keyword = KW_INT;
	else if (!strCmpConstStr(str, "package")) token->attribute.keyword = KW_PACKAGE;
	else if (!strCmpConstStr(str, "return")) token->attribute.keyword = KW_RETURN;
	else if (!strCmpConstStr(str, "string")) token->attribute.keyword = KW_STRING;
	else token->type = TOKEN_IDENTIFIER;

    if (token->type != TOKEN_IDENTIFIER) {
        token->type = TOKEN_KEYWORD;
        return freeResources(ERROR_CODE_OK, str);
    }

    if (!strCmpString(str, token->attribute.string)) {
        return freeResources(ERROR_INTERNAL, str);
    }

    return freeResources(ERROR_CODE_OK, str);
}


static int proscessingInteger(Token* token, string* str) {
    char *endptr;

    int value = (int)strtol(str->str, &endptr, 10);
    if (*endptr) {
        return freeResources(ERROR_INTERNAL, str);
    }
    token->attribute.integer = value;
    token->type = TOKEN_INT;
    return freeResources(ERROR_CODE_OK, str);
}

static int processingDecimal(Token* token, string* str) {
    char* endptr;

    float value = strtof(str->str, &endptr);
    if (*endptr) {
        return freeResources(ERROR_CODE_OK, str);
    }
    token->attribute.decimal = value;
    token->type = TOKEN_FLOAT;
    return freeResources(ERROR_CODE_OK, str);
}

int getNextToken(Token* token) {

    if (sourceFile == NULL) {
        return ERROR_INTERNAL;
    }

    if (setStr == NULL) {
        return ERROR_INTERNAL;
    }

    token->attribute.string = setStr;

    string str_ing;
    string* str = &str_ing;
    if (strInit(str)) {
        return ERROR_INTERNAL;
    }

    int state = SCANNER_STATE_START;
    token->type = TOKEN_NOTHING;

    char c, * endptr, strnumber[4] = { 0 };

    while (true) {
        c = (char)getc(sourceFile);

        switch (state) {
        case (SCANNER_STATE_START):
            if (isspace(c)) {
                state = SCANNER_STATE_START;
            }
            else if (c == '\n') {
                state = SCANNER_STATE_EOL;
            }
            else if (c == '"') {
                state = SCANNER_STATE_STRING;
            }
            else if (c == '+') {
                token->type = TOKEN_PLUS;
                return freeResources(ERROR_CODE_OK, str);
            }
            else if (c == '-') {
                token->type = TOKEN_MINUS;
                return freeResources(ERROR_CODE_OK, str);
            }
            else if (c == '*') {
                token->type = TOKEN_MUL;
                return freeResources(ERROR_CODE_OK, str);
            }
            else if (c == '(') {
                token->type = TOKEN_LBRACKET;
                return freeResources(ERROR_CODE_OK, str);
            }
            else if (c == ')') {
                token->type = TOKEN_RBRACKET;
                return freeResources(ERROR_CODE_OK, str);
            }
            else if (c == ',') {
                token->type = TOKEN_COMMA;
                return freeResources(ERROR_CODE_OK, str);
            }
            else if (c == ';') {
                token->type = TOKEN_SEMICOLON;
                return freeResources(ERROR_CODE_OK, str);
            }
            else if (c == '<') {
                state = SCANNER_STATE_LESS_THAN;
            }
            else if (c == '>') {
                state = SCANNER_STATE_MORE_THAN;
            }
            else if (c == ':') {
                state = SCANNER_STATE_ASSIGN;
            }
            else if (c == '!') {
                state = SCANNER_STATE_NOT_EQUAL;
            }
            else if (c == '=') {
                state = SCANNER_STATE_EQUAL;
            }
            else if (c == '/') {
                state = SCANNER_STATE_COMMENTARY;
            }
            else if (isalpha(c) || c == '_') {
                if (!strAddChar(str, (char)tolower(c))) {
                    return freeResources(ERROR_INTERNAL, str);
                }
                state = SCANNER_STATE_KEYWORD_OR_IDENTIFIER;
            }
            else if (isdigit(c)) {
                if (!strAddChar(str, c)) {
                    return freeResources(ERROR_INTERNAL, str);
                }
                state = SCANNER_STATE_NUMBER;
            }
            else if (c == EOF) {
                token->type = TOKEL_EOF;
                return freeResources(ERROR_CODE_OK, str);
            }
            else {
                return freeResources(ERROR_LEX, str);
            }
            break;

        case (SCANNER_STATE_NUMBER):
            if (isdigit(c)) {
                if (!strAddChar(str, c)) {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else if (c == '.') {
                state = SCANNER_STATE_DECIMAL_POINT;
                if (!strAddChar(str, c)) {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else if (tolower(c) == 'e') {
                state = SCANNER_STATE_EXPONENT;
                if (!strAddChar(str, c)) {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else {
                ungetc(c, sourceFile);
                return proscessingInteger(token, str);
            }
            break;

        case (SCANNER_STATE_DECIMAL_POINT):
            if (isdigit(c)) {
                state = SCANNER_STATE_FLOAT;
                if (!strAddChar(str, c)) {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else {
                return freeResources(ERROR_LEX, str);
            }
            break;

        case(SCANNER_STATE_FLOAT):
            if (isdigit(c)) {
                if (!strAddChar(str, c)) {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else if (tolower(c) == 'e') {
                state = SCANNER_STATE_EXPONENT;
                if (!strAddChar(str, c)) {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else {
                ungetc(c, sourceFile);
                return processingDecimal(token, str);
            }
            break;

        case (SCANNER_STATE_EXPONENT):
            if (isdigit(c)) {
                state = SCANNER_STATE_FINAL_NUMBER;
                if (!strAddChar(str, c)) {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else if (c == '+' || c == '-') {
                state = SCANNER_STATE_EXPONENT_SIGNED;
                if (!strAddChar(str, c)) {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else {
                return freeResources(ERROR_LEX, str);
            }
            break;

        case(SCANNER_STATE_EXPONENT_SIGNED):
            if (isdigit(c)) {
                state = SCANNER_STATE_FINAL_NUMBER;
                if (!strAddChar(str, c)) {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else {
                return freeResources(ERROR_LEX, str);
            }
            break;

        case (SCANNER_STATE_FINAL_NUMBER):
            if (isdigit(c)) {
                if (!strAddChar(str, c)) {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else {
                ungetc(c, sourceFile);
                return processingDecimal(token, str);
            }
            break;

        case (SCANNER_STATE_STRING):
            if (c < 32) {
                return freeResources(ERROR_LEX, str);
            }
            else if (c == '"') {
                if (!strCopyString(str, token->attribute.string)) {
                    return freeResources(ERROR_INTERNAL, str);
                }
                token->type = TOKEN_STRING;
                return freeResources(ERROR_CODE_OK, str);
            }
            else if (c == '\\') {
                state = SCANNER_STATE_ESCAPE;
            }
            else {
                if (!strAddChar(str, c)) {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            break;

        case(SCANNER_STATE_ESCAPE):
            if (c < 32) {
                return freeResources(ERROR_LEX, str);
            }
            else if (c == '"') {
                c = '"';
                if (!strAddChar(str, c)) {
                    return freeResources(ERROR_INTERNAL, str);
                }
                state = SCANNER_STATE_STRING;
            }
            else if (c == 'n') {
                c = '\n';
                if (!strAddChar(str, c)) {
                    return freeResources(ERROR_INTERNAL, str);
                }
                state = SCANNER_STATE_STRING;
            }
            else if (c == 't') {
                c = '\t';
                if (!strAddChar(str, c)) {
                    return freeResources(ERROR_INTERNAL, str);
                }
                state = SCANNER_STATE_STRING;
            }
            else if (c == '\\') {
                c = '\\';
                if (!strAddChar(str, c)) {
                    return freeResources(ERROR_INTERNAL, str);
                }
                state = SCANNER_STATE_STRING;
            }
            else {
                return freeResources(ERROR_LEX, str);
            }
            break;

        case (SCANNER_STATE_LESS_THAN):
            if (c == '=') {
                token->type = TOKEN_LEQ;
            }
            else {
                ungetc(c, sourceFile);
                token->type = TOKEN_LT;
            }
            return freeResources(ERROR_CODE_OK, str);

        case(SCANNER_STATE_MORE_THAN):
            if (c == '=') {
                token->type = TOKEN_GEQ;
            }
            else {
                ungetc(c, sourceFile);
                token->type = TOKEN_GT;
            }
            return freeResources(ERROR_CODE_OK, str);

        case (SCANNER_STATE_ASSIGN):
            if (c == '=') {
                token->type = TOKEN_ASSIGN;
            }
            else {
                ungetc(c, sourceFile);
 //VYRIEŠIŤ
            }

        }//switch
    }//while
}//funkcia
