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
#define SCANNER_STATE_STRING 10
#define SCANNER_STATE_ESCAPE 11
#define SCANNER_STATE_LESS_THAN 12
#define SCANNER_STATE_MORE_THAN 13
#define SCANNER_STATE_COMMENTARY 14
#define SCANNER_STATE_BLOCK_COMMENTARY_START 15
#define SCANNER_STATE_BLOCK_COMMENTARY_EXIT 16
#define SCANNER_STATE_ASSIGN 17
#define SCANNER_STATE_EQUAL 18
#define SCANNER_STATE_NOT_EQUAL 19
#define SCANNER_STATE_SLASH 20
#define SCANNER_STATE_VARIABLE_DEF 21
#define SCANNER_STATE_CHARACTER 22
#define SCANNER_STATE_CHARACTER_SECOND 23

FILE *sourceFile;
string *setStr;

void setSourceFile(FILE *f)
{
    sourceFile = f;
}

static int freeResources(int exitCode, string *str)
{
    strFree(str);
    return exitCode;
}

static int processingKeywordIdentifier(string *str, Token *token)
{
    if (strCmpConstStr(str, "else") == 0)
        token->attribute.keyword = KW_ELSE;
    else if (strCmpConstStr(str, "float64") == 0)
        token->attribute.keyword = KW_FLOAT64;
    else if (strCmpConstStr(str, "for") == 0)
        token->attribute.keyword = KW_FOR;
    else if (strCmpConstStr(str, "func") == 0)
        token->attribute.keyword = KW_FUNC;
    else if (strCmpConstStr(str, "if") == 0)
        token->attribute.keyword = KW_IF;
    else if (strCmpConstStr(str, "int") == 0)
        token->attribute.keyword = KW_INT;
    else if (strCmpConstStr(str, "package") == 0)
        token->attribute.keyword = KW_PACKAGE;
    else if (strCmpConstStr(str, "return") == 0)
        token->attribute.keyword = KW_RETURN;
    else if (strCmpConstStr(str, "string") == 0)
        token->attribute.keyword = KW_STRING;
    else
        token->type = TOKEN_IDENTIFIER;

    if (token->type != TOKEN_IDENTIFIER)
    {
        token->type = TOKEN_KEYWORD;
        return freeResources(ERROR_CODE_OK, str);
    }

    // TODO - memory leak - na 0. odevzdání jsem to zakomentoval, zatím to není potřeba
    // if (strCopyString(token->attribute.string, str) == 1)
    // {
    //     return freeResources(ERROR_INTERNAL, str);
    // }

    return freeResources(ERROR_CODE_OK, str);
}

static int proscessingInteger(Token *token, string *str)
{
    char *endptr;

    int value = (int)strtol(str->str, &endptr, 10);
    if (*endptr)
    {
        return freeResources(ERROR_INTERNAL, str);
    }
    token->attribute.integer = value;
    token->type = TOKEN_INT;
    return freeResources(ERROR_CODE_OK, str);
}

static int processingDecimal(Token *token, string *str)
{
    char *endptr;

    float value = strtof(str->str, &endptr);
    if (*endptr)
    {
        return freeResources(ERROR_CODE_OK, str);
    }
    token->attribute.decimal = value;
    token->type = TOKEN_FLOAT;
    return freeResources(ERROR_CODE_OK, str);
}

int getNextToken(Token *token)
{

    if (sourceFile == NULL)
    {
        return ERROR_INTERNAL;
    }

    // TODO - memory leak - na 0. odevzdání jsem to zakomentoval, zatím to není potřeba
    // string strToken;
    // if (strInit(&strToken))
    //     return ERROR_INTERNAL;
    // token->attribute.string = &strToken;

    string stringstr;
    string *str = &stringstr;
    if (strInit(str))
    {
        return ERROR_INTERNAL;
    }

    int state = SCANNER_STATE_START;
    token->type = TOKEN_NOTHING;

    char c, character[2] = {0};

    while (true)
    {
        c = (char)getc(sourceFile);

        switch (state)
        {
        case (SCANNER_STATE_START):
            if (c == '\n')
            {
                state = SCANNER_STATE_EOL;
            }
            else if (isspace(c))
            {
                state = SCANNER_STATE_START;
            }
            else if (c == '+')
            {
                token->type = TOKEN_PLUS;
                return freeResources(ERROR_CODE_OK, str);
            }
            else if (c == '-')
            {
                token->type = TOKEN_MINUS;
                return freeResources(ERROR_CODE_OK, str);
            }
            else if (c == '*')
            {
                token->type = TOKEN_MUL;
                return freeResources(ERROR_CODE_OK, str);
            }
            else if (c == '/')
            {
                state = SCANNER_STATE_SLASH;
            }
            else if (c == '<')
            {
                state = SCANNER_STATE_LESS_THAN;
            }
            else if (c == '>')
            {
                state = SCANNER_STATE_MORE_THAN;
            }
            else if (c == '=')
            {
                state = SCANNER_STATE_EQUAL;
            }
            else if (c == '!')
            {
                state = SCANNER_STATE_NOT_EQUAL;
            }
            else if (c == '(')
            {
                token->type = TOKEN_LBRACKET;
                return freeResources(ERROR_CODE_OK, str);
            }
            else if (c == ')')
            {
                token->type = TOKEN_RBRACKET;
                return freeResources(ERROR_CODE_OK, str);
            }
            else if (c == '{')
            {
                token->type = TOKEN_LCURLYBRACKET;
                return freeResources(ERROR_CODE_OK, str);
            }
            else if (c == '}')
            {
                token->type = TOKEN_RCURLYBRACKET;
                return freeResources(ERROR_CODE_OK, str);
            }
            else if (c == ',')
            {
                token->type = TOKEN_COMMA;
                return freeResources(ERROR_CODE_OK, str);
            }
            else if (c == ';')
            {
                token->type = TOKEN_SEMICOLON;
                return freeResources(ERROR_CODE_OK, str);
            }
            else if (c == ':')
            {
                state = SCANNER_STATE_VARIABLE_DEF;
            }
            else if (isalpha(c) || c == '_')
            {
                if (strAddChar(str, (char)tolower(c)) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
                state = SCANNER_STATE_KEYWORD_OR_IDENTIFIER;
            }
            else if (isdigit(c))
            {
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
                state = SCANNER_STATE_NUMBER;
            }
            else if (c == '"')
            {
                state = SCANNER_STATE_STRING;
            }
            else if (c == EOF)
            {
                token->type = TOKEN_EOF;
                return freeResources(ERROR_CODE_OK, str);
            }
            else
            {
                return freeResources(ERROR_LEX, str);
            }
            break;

        case (SCANNER_STATE_LESS_THAN):
            if (c == '=')
            {
                token->type = TOKEN_LEQ;
            }
            else
            {
                ungetc(c, sourceFile);
                token->type = TOKEN_LT;
            }
            return freeResources(ERROR_CODE_OK, str);

        case (SCANNER_STATE_MORE_THAN):
            if (c == '=')
            {
                token->type = TOKEN_GEQ;
            }
            else
            {
                ungetc(c, sourceFile);
                token->type = TOKEN_GT;
            }
            return freeResources(ERROR_CODE_OK, str);

        case (SCANNER_STATE_EQUAL):
            if (c == '=')
            {
                token->type = TOKEN_EQ;
            }
            else
            {
                ungetc(c, sourceFile);
                token->type = TOKEN_ASSIGN;
            }
            return freeResources(ERROR_CODE_OK, str);

        case (SCANNER_STATE_NOT_EQUAL):
            if (c == '=')
            {
                token->type = TOKEN_NEQ;
                return freeResources(ERROR_CODE_OK, str);
            }
            else
            {
                ungetc(c, sourceFile);
                return freeResources(ERROR_LEX, str);
            }
            break;

        case (SCANNER_STATE_VARIABLE_DEF):
            if (c == '=')
            {
                token->type = TOKEN_VAR_DEF;
                return freeResources(ERROR_CODE_OK, str);
            }
            else
            {
                ungetc(c, sourceFile);
                return freeResources(ERROR_LEX, str);
            }
            break;

        case (SCANNER_STATE_KEYWORD_OR_IDENTIFIER):
            if (isalnum(c) || c == '_')
            {
                if (strAddChar(str, (char)tolower(c)) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else
            {
                ungetc(c, sourceFile);
                return processingKeywordIdentifier(str, token);
            }
            break;

        case (SCANNER_STATE_NUMBER):
            if (isdigit(c))
            {
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else if (c == '.')
            {
                state = SCANNER_STATE_DECIMAL_POINT;
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else if (tolower(c) == 'e')
            {
                state = SCANNER_STATE_EXPONENT;
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else
            {
                ungetc(c, sourceFile);
                return proscessingInteger(token, str);
            }
            break;

        case (SCANNER_STATE_DECIMAL_POINT):
            if (isdigit(c))
            {
                state = SCANNER_STATE_FLOAT;
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else
            {
                return freeResources(ERROR_LEX, str);
            }
            break;

        case (SCANNER_STATE_FLOAT):
            if (isdigit(c))
            {
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else if (tolower(c) == 'e')
            {
                state = SCANNER_STATE_EXPONENT;
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else
            {
                ungetc(c, sourceFile);
                return processingDecimal(token, str);
            }
            break;

        case (SCANNER_STATE_EXPONENT):
            if (isdigit(c))
            {
                state = SCANNER_STATE_FINAL_NUMBER;
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else if (c == '+' || c == '-')
            {
                state = SCANNER_STATE_EXPONENT_SIGNED;
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else
            {
                return freeResources(ERROR_LEX, str);
            }
            break;

        case (SCANNER_STATE_EXPONENT_SIGNED):
            if (isdigit(c))
            {
                state = SCANNER_STATE_FINAL_NUMBER;
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else
            {
                return freeResources(ERROR_LEX, str);
            }
            break;

        case (SCANNER_STATE_FINAL_NUMBER):
            if (isdigit(c))
            {
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else
            {
                ungetc(c, sourceFile);
                return processingDecimal(token, str);
            }
            break;

        case (SCANNER_STATE_STRING):
            if (c < 32)
            {
                return freeResources(ERROR_LEX, str);
            }
            else if (c == '"')
            {
                // TODO - memory leak - na 0. odevzdání jsem to zakomentoval, zatím to není potřeba
                // if (strCopyString(token->attribute.string, str) == 1)
                // {
                //     return freeResources(ERROR_INTERNAL, str);
                // }
                token->type = TOKEN_STRING;
                return freeResources(ERROR_CODE_OK, str);
            }
            else if (c == '\\')
            {
                state = SCANNER_STATE_ESCAPE;
            }
            else
            {
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            break;

        case (SCANNER_STATE_ESCAPE):
            if (c < 32)
            {
                return freeResources(ERROR_LEX, str);
            }
            else if (c == '"')
            {
                c = '"';
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
                state = SCANNER_STATE_STRING;
            }
            else if (c == 'n')
            {
                c = '\n';
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
                state = SCANNER_STATE_STRING;
            }
            else if (c == 't')
            {
                c = '\t';
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
                state = SCANNER_STATE_STRING;
            }
            else if (c == '\\')
            {
                c = '\\';
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
                state = SCANNER_STATE_STRING;
            }
            else if (c == 'x')
            {
                state = SCANNER_STATE_CHARACTER;
            }
            else
            {
                return freeResources(ERROR_LEX, str);
            }
            break;

        case (SCANNER_STATE_CHARACTER):
            if (c > 47 && c < 58)
            {
                character[0] = c;
                state = SCANNER_STATE_CHARACTER_SECOND;
            }
            else if (c > 64 && c < 71)
            {
                character[0] = c;
                state = SCANNER_STATE_CHARACTER_SECOND;
            }
            else if (c > 96 && c < 103)
            {
                character[0] = c;
                state = SCANNER_STATE_CHARACTER_SECOND;
            }
            else
            {
                return freeResources(ERROR_LEX, str);
            }
            break;

        case (SCANNER_STATE_CHARACTER_SECOND):
            if (c > 47 && c < 58)
            {
                character[1] = c;
                int val = (int)strtol(character, NULL, 16);
                c = (char)val;
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
                state = SCANNER_STATE_STRING;
            }
            else if (c > 64 && c < 71)
            {
                character[1] = c;
                int val = (int)strtol(character, NULL, 16);
                c = (char)val;
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
                state = SCANNER_STATE_STRING;
            }
            else if (c > 96 && c < 103)
            {
                character[1] = c;
                int val = (int)strtol(character, NULL, 16);
                c = (char)val;
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
                state = SCANNER_STATE_STRING;
            }
            else
            {
                return freeResources(ERROR_LEX, str);
            }
            break;

        case (SCANNER_STATE_SLASH):
            if (c == '/')
            {
                state = SCANNER_STATE_COMMENTARY;
            }
            else if (c == '*')
            {
                state = SCANNER_STATE_BLOCK_COMMENTARY_START;
            }
            else
            {
                ungetc(c, sourceFile);
                token->type = TOKEN_DIV;
                return freeResources(ERROR_CODE_OK, str);
            }
            break;

        case (SCANNER_STATE_COMMENTARY):
            while (c != '\n')
            {
                c = (char)getc(sourceFile);
                if (c == EOF)
                {
                    state = SCANNER_STATE_START;
                    ungetc(c, sourceFile);
                    break;
                }
            }
            if (c == '\n')
            {
                state = SCANNER_STATE_START;
                ungetc(c, sourceFile);
            }
            break;

        case (SCANNER_STATE_BLOCK_COMMENTARY_START):
            if (c == '*')
            {
                state = SCANNER_STATE_BLOCK_COMMENTARY_EXIT;
            }
            else if (c == EOF)
            {
                return freeResources(ERROR_LEX, str);
            }
            break;

        case (SCANNER_STATE_BLOCK_COMMENTARY_EXIT):
            if (c == EOF)
            {
                return freeResources(ERROR_LEX, str);
            }
            else if (c == '/')
            {
                state = SCANNER_STATE_START;
            }
            else if (c == '*')
            {
                state = SCANNER_STATE_BLOCK_COMMENTARY_EXIT;
            }
            else
            {
                state = SCANNER_STATE_BLOCK_COMMENTARY_START;
            }
            break;

        case (SCANNER_STATE_EOL):
            if (isspace(c))
            {
                break;
            }
            ungetc(c, sourceFile);
            token->type = TOKEN_EOL;
            return freeResources(ERROR_CODE_OK, str);

        } //switch
    }     //while
} //funkcia
