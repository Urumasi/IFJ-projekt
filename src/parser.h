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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "scanner.h"
#include "symtable.h"
#include "stack.h"
#include "error.h"
#include "str.h"
#include "error.h"

typedef struct parser
{
    Token token;
    string id;
    int returnCode;
    bool tokenProcessed;
    bool declaredMain;
    bool missingReturn;
    bool funcInExpr;
    DataType exprType;
    DataType idType;
    bool exprIsBool;
    bool exprBoolAllowed;

    tSymtable sGlobal;
    tSymStack sLocal;

    int scopeCounter;
    int typeCounter;
    tSymtableData currentFunc;
    tSymtableData currentID;
    tSymtableData calledFunc;
    string typesLeft;
    string typesRight;

} * Parser;

void addType(char type, string *typesList, tSymtableData data, Parser parser);
int addBuiltinFunc(char *key, char *argumentTypes, char *returnTypes, Parser parser);
DataType keywordToType(Keyword keyword);
char typeToChar(DataType type);
bool compareTypes(string typesLeft, string typesRight);
Parser initParser();
void deleteParser(Parser parser);
int parse();

int package(Parser parser);
int prog(Parser parser);
int params(Parser parser);
int params_n(Parser parser);
int ret(Parser parser);
int ret_params(Parser parser);
int ret_params_n(Parser parser);
int type(Parser parser);
int body(Parser parser);
int body_n(Parser parser);
int id_n(Parser parser);
int for_definition(Parser parser);
int for_assign(Parser parser);
int value(Parser parser);
int expression_n(Parser parser);
int definition(Parser parser);
int assign(Parser parser);
int func(Parser parser);
int arg(Parser parser);
int term(Parser parser);
int term_n(Parser parser);
int ret_values(Parser parser);

/**
 * @brief Gets new token if token isn't processed. Returns error code if anything fails.
 *        Unprocessed token occurs in "<rule> -> eps" rule, meaning that current token 
 *        is from another rule and in next getToken() call, token remains the same
 *  
 */
#define getToken()                                                                    \
    do                                                                                \
    {                                                                                 \
        if (parser->tokenProcessed)                                                   \
        {                                                                             \
            if ((parser->returnCode = getNextToken(&parser->token)) != ERROR_CODE_OK) \
                return parser->returnCode;                                            \
        }                                                                             \
        else                                                                          \
            parser->tokenProcessed = true;                                            \
    } while (0)

/**
 * @brief Checks if token type is same as argument
 * 
 */
#define isType(_token) (parser->token.type == _token)

/**
 * @brief Gets new token and checks type. Returns syntax parser->returnCode code, if types don't match
 * 
 */
#define getType(_token)       \
    do                        \
    {                         \
        getToken();           \
        if (!isType(_token))  \
            return ERROR_SYN; \
    } while (0)

/**
 * @brief Checks if token type is keyword and then checks if token attribute is same as argument
 * 
 */
#define isKeyword(_keyword) ((parser->token.type == TOKEN_KEYWORD) && (parser->token.attribute.keyword == _keyword))

/**
 * @brief Gets new token, checks if token type is keyword ane then checks token attribute. Returns syntax parser->returnCode code, if keywords don't match
 * 
 */
#define getKeyword(_keyword)                             \
    do                                                   \
    {                                                    \
        getType(TOKEN_KEYWORD);                          \
        if (parser->token.attribute.keyword != _keyword) \
            return ERROR_SYN;                            \
    } while (0)

/**
 * @brief Calls function in argument. If function doesn't return 0, returns that value.
 * 
 */
#define getRule(rule)                      \
    do                                     \
    {                                      \
        parser->returnCode = rule(parser); \
        checkReturn();                     \
    } while (0)

/**
 * @brief Returns syntax error if rule fails, OK code if not
 * 
 */
#define returnRule()           \
    else { return ERROR_SYN; } \
    return ERROR_CODE_OK

/**
 * @brief Returns parser->returnCode, if it isn't OK code
 * 
 */
#define checkReturn()                            \
    do                                           \
    {                                            \
        if (parser->returnCode != ERROR_CODE_OK) \
            return parser->returnCode;           \
    } while (0)

/**
 * @brief Returns intertal error, if pointer is null
 * 
 */
#define symCheckNull(data)         \
    do                             \
    {                              \
        if (data == NULL)          \
            return ERROR_INTERNAL; \
    } while (0)

/**
 * @brief Returns semantic error, if record in symtable is defined
 * 
 */
#define symCheckDefined(data) \
    do                        \
    {                         \
        if (data->defined)    \
            return ERROR_SEM; \
    } while (0)

/**
 * @brief Returns semantic error, if record in symtable is not found
 * 
 */
#define symCheckFound(data)   \
    do                        \
    {                         \
        if (data == NULL)     \
            return ERROR_SEM; \
    } while (0)

/**
 * @brief Inserts record to global symtable
 * 
 */
#define symInsertGlobal(key)                                         \
    do                                                               \
    {                                                                \
        parser->currentFunc = symtableInsert(&parser->sGlobal, key); \
        symCheckNull(parser->currentFunc);                           \
        symCheckDefined(parser->currentFunc);                        \
        parser->currentFunc->defined = true;                         \
    } while (0)

/**
 * @brief Inserts record to local symtable on top of the stack
 * 
 */
#define symInsertLocal(key)                                                     \
    do                                                                          \
    {                                                                           \
        parser->currentID = symtableInsert(&parser->sLocal.top->symtable, key); \
        symCheckNull(parser->currentID);                                        \
        symCheckDefined(parser->currentID);                                     \
        parser->currentID->defined = true;                                      \
    } while (0)

/**
 * @brief Finds record in global symtable
 * 
 */
#define symReadGlobal(key)                                         \
    do                                                             \
    {                                                              \
        parser->currentFunc = symtableRead(&parser->sGlobal, key); \
        symCheckFound(parser->currentFunc);                        \
    } while (0)

/**
 * @brief Finds record in local symtable on top of the stack
 * 
 */
#define symReadLocal(key)                                            \
    do                                                               \
    {                                                                \
        parser->currentID = symtableReadStack(&parser->sLocal, key); \
        symCheckFound(parser->currentID);                            \
    } while (0)

#endif
