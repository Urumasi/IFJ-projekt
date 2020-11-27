/**
 * IFJ Project 2020: Team 008, variant II
 * 
 * @file parser.c
 * 
 * @brief
 *
 * 
 * @author Martin Kneslík <xknesl02@stud.fit.vutbr.cz>
 * @author Adriana Jurkechová <xjurke02@stud.fit.vutbr.cz>
 * @author Karel Norek <xnorek01@stud.fit.vutbr.cz>
 * @author Petr Salaba <xsalab00@stud.fit.vutbr.cz>
 */

#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdbool.h>
#include "symtable.h"
#include "stack.h"

typedef struct parser
{
    Token token;
    int returnCode;
    bool tokenProcessed;
    bool declaredMain;
    bool missingReturn;
    bool inScope;
    int countScope;
    bool funcInExpr;
    DataType exprType;
    bool exprIsBool;
    bool exprBoolAllowed;

    int countLeft;
    int countRight;

    string str1;
    string str2;

    tSymtable sGlobal;
    tSymStack sLocal;

    tSymtableData *sData;

} Parser;

int initParser(Parser *parser);
void deleteParser(Parser *parser);
int parse();

int package(Parser *parser);
int prog(Parser *parser);
int params(Parser *parser);
int params_n(Parser *parser);
int ret(Parser *parser);
int ret_params(Parser *parser);
int ret_params_n(Parser *parser);
int type(Parser *parser);
int body(Parser *parser);
int body_n(Parser *parser);
int id_n(Parser *parser);
int for_definition(Parser *parser);
int for_assign(Parser *parser);
int value(Parser *parser);
int expression_n(Parser *parser);
int definition(Parser *parser);
int assign(Parser *parser);
int func(Parser *parser);
int arg(Parser *parser);
int term(Parser *parser);
int term_n(Parser *parser);
int ret_values(Parser *parser);

/**
 * @brief Gets new token if token isn't processed. Returns parser->returnCode code if anything fails.
 *        Unprocessed token occurs in "<rule> -> eps" rule, meaning that current token 
 *        is from another rule and in next getToken() call, token remains the same
 *  
 */
#define getToken()                                                                \
    if (parser->tokenProcessed)                                                   \
    {                                                                             \
        if ((parser->returnCode = getNextToken(&parser->token)) != ERROR_CODE_OK) \
            return parser->returnCode;                                            \
    }                                                                             \
    else                                                                          \
        parser->tokenProcessed = true

/**
 * @brief Checks if token type is same as argument
 * 
 */
#define isType(_token) (parser->token.type == _token)

/**
 * @brief Gets new token and checks type. Returns syntax parser->returnCode code, if types don't match
 * 
 */
#define getType(_token)  \
    getToken();          \
    if (!isType(_token)) \
        return ERROR_SYN;

/**
 * @brief Checks if token type is keyword and then checks if token attribute is same as argument
 * 
 */
#define isKeyword(_keyword) ((parser->token.type == TOKEN_KEYWORD) && (parser->token.attribute.keyword == _keyword))

/**
 * @brief Gets new token, checks if token type is keyword ane then checks token attribute. Returns syntax parser->returnCode code, if keywords don't match
 * 
 */
#define getKeyword(_keyword)                         \
    getType(TOKEN_KEYWORD);                          \
    if (parser->token.attribute.keyword != _keyword) \
        return ERROR_SYN;

/**
 * @brief Calls function in argument. If function doesn't return 0, returns that value.
 * 
 */
#define getRule(_rule)                       \
    parser->returnCode = _rule(parser);      \
    if (parser->returnCode != ERROR_CODE_OK) \
    return parser->returnCode

#define returnRule()           \
    else { return ERROR_SYN; } \
    return ERROR_CODE_OK

#endif
