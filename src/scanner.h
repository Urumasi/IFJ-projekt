/**
 * IFJ Project 2020: Team 008, variant II
 *
 * @file scanner.h
 *
 * @brief Scanner interface
 *
 *
 * @author Martin Kneslík <xknesl02@stud.fit.vutbr.cz>
 * @author Adriana Jurkechová <xjurke02@stud.fit.vutbr.cz>
 * @author Karel Norek <xnorek01@stud.fit.vutbr.cz>
 * @author Petr Salaba <xsalab00@stud.fit.vutbr.cz>
 */

#ifndef __SCANNER_H__
#define __SCANNER_H__

#include <stdio.h>
#include <stdint.h>

#include "error.h"
#include "str.h"

/**
  * @enum Reserved keywords.
  */
typedef enum
{
    KW_ELSE,
    KW_FLOAT64,
    KW_FOR,
    KW_FUNC,
    KW_IF,
    KW_INT,
    KW_PACKAGE,
    KW_RETURN,
    KW_STRING,
} Keyword;

/**
 * @union Token attribute.
 */
typedef union
{
    Keyword keyword;
    string *string;
    int64_t integer;
    double decimal;
} Token_attribute;

/**
 * @enum Token type.
 */
typedef enum
{
    TOKEN_EQ,
    TOKEN_NEQ,
    TOKEN_LEQ,
    TOKEN_GEQ,
    TOKEN_LT,
    TOKEN_GT,

    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MUL,
    TOKEN_DIV,

    TOKEN_ASSIGN,
    TOKEN_VAR_DEF,
    TOKEN_COMMA,
    TOKEN_SEMICOLON,
    TOKEN_RBRACKET,
    TOKEN_LBRACKET,
    TOKEN_RCURLYBRACKET,
    TOKEN_LCURLYBRACKET,

    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_EOL,
    TOKEN_EOF,
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_STRING,
    TOKEN_NOTHING,
} Token_type;

/**
 * @struct Token.
 */
typedef struct
{
    Token_type type;
    Token_attribute attribute;
} Token;

void setSourceFile(FILE *f);
void setString(string *s);
int getNextToken(Token *token);

#endif //__SCANNER_H__
