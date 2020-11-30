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

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "scanner.h"
#include "parser.h"
#include "expression.h"
#include "error.h"
#include "symtable.h"

#define symCheckNull(data) \
    if (data == NULL)      \
    return ERROR_INTERNAL

#define symCheckDefined(data) \
    if (data->defined)        \
    return ERROR_SEM

#define symCheckFound(data) \
    if (data == NULL)       \
    return ERROR_SEM

#define symInsertGlobal(key)                                     \
    parser->currentFunc = symtableInsert(&parser->sGlobal, key); \
    symCheckNull(parser->currentFunc);                           \
    symCheckDefined(parser->currentFunc);                        \
    parser->currentFunc->defined = true

#define symInsertLocal(key)                                                 \
    parser->currentID = symtableInsert(&parser->sLocal.top->symtable, key); \
    symCheckNull(parser->currentID)

#define symReadGlobal(key)                                     \
    parser->currentFunc = symtableRead(&parser->sGlobal, key); \
    symCheckNull(parser->currentFunc)

#define symReadLocal(key)                                       \
    parser->currentID = symtableReadStack(parser->sLocal, key); \
    symCheckFound(parser->currentFunc)

Parser parser;

Parser initParser()
{
    Parser parser = malloc(sizeof(struct parser));
    parser->tokenProcessed = true;
    parser->funcInExpr = false;
    parser->countLeft = 1;
    parser->countRight = 1;
    parser->exprType = tNONE;
    parser->exprIsBool = false;
    parser->exprBoolAllowed = false;
    symtableInit(&parser->sGlobal);
    symStackInit(&parser->sLocal);
    return parser;
}

void deleteParser(Parser parser)
{
    symtableClearAll(&parser->sGlobal);
    symStackDispose(&parser->sLocal);
    free(parser);
}

/**
 * @brief Main function
 * 
 * @return Error code of parser
 */
int parse()
{
    Parser parser = initParser();
    parser->returnCode = package(parser);
    if (parser->returnCode == ERROR_CODE_OK)
    {
        //TODO
    }
    int returnCode = parser->returnCode;
    deleteParser(parser);
    return returnCode;
}

/**
 * @brief <package> rule
 * 
 * @return Error code of rule 
 */
int package(Parser parser)
{
    getToken();
    while (isType(TOKEN_EOL))
        getToken();
    parser->tokenProcessed = false;

    // <package> -> PACKAGE ID EOL <prog>
    getKeyword(KW_PACKAGE);
    getType(TOKEN_IDENTIFIER);
    // only identifier "main" can follow after keyword package
    if (strCmpConstStr(parser->token.attribute.string, "main"))
        return ERROR_SYN;
    getType(TOKEN_EOL);
    getRule(prog);
    return ERROR_CODE_OK;
}

/**
 * @brief <prog> rule
 * 
 * @return Error code of rule 
 */
int prog(Parser parser)
{
    getToken();
    // <prog> -> FUNC ID ( <params> ) <ret> { EOL <body> } EOL <prog>
    if (isKeyword(KW_FUNC))
    {
        getType(TOKEN_IDENTIFIER);
        symInsertGlobal(parser->token.attribute.string->str);
        getType(TOKEN_LBRACKET);
        getRule(params);
        getType(TOKEN_RBRACKET);
        getRule(ret);
        getType(TOKEN_LCURLYBRACKET);
        getType(TOKEN_EOL);
        getRule(body);
        getType(TOKEN_RCURLYBRACKET);
        getType(TOKEN_EOL);
        getRule(prog);
    }
    //<prog> -> EOF
    else if (isType(TOKEN_EOF))
    {
    }
    else if (isType(TOKEN_EOL))
    {
        getRule(prog);
    }
    returnRule();
}

/**
 * @brief <params> rule
 * 
 * @return Error code of rule 
 */
int params(Parser parser)
{
    getToken();
    // <params> -> ID <type> <params_n>
    if (isType(TOKEN_IDENTIFIER))
    {
        getRule(type);
        getRule(params_n);
    }
    // <params> -> ε
    else if (isType(TOKEN_RBRACKET))
    {
        parser->tokenProcessed = false;
    }
    returnRule();
}

/**
 * @brief <type> rule
 * 
 * @return Error code of rule 
 */
int type(Parser parser)
{
    // TODO - semantic
    getToken();
    if (isKeyword(KW_INT))
    {
        strAddChar(&parser->currentFunc->argumentTypes, '0');
    }
    else if (isKeyword(KW_FLOAT64))
    {
        strAddChar(&parser->currentFunc->argumentTypes, '1');
    }
    else if (isKeyword(KW_STRING))
    {
        strAddChar(&parser->currentFunc->argumentTypes, '2');
    }
    returnRule();
}

/**
 * @brief <params_n> rule
 * 
 * @return Error code of rule 
 */
int params_n(Parser parser)
{
    getToken();
    // <params_n> , ID <type> <params_n>
    if (isType(TOKEN_COMMA))
    {
        getType(TOKEN_IDENTIFIER);
        getRule(type);
        getRule(params_n);
    }
    // <params_n> -> ε
    else if (isType(TOKEN_RBRACKET))
    {
        parser->tokenProcessed = false;
    }
    returnRule();
}

/**
 * @brief <ret> rule
 * 
 * @return Error code of rule 
 */
int ret(Parser parser)
{
    getToken();
    // <ret> -> ( <ret_params> )
    if (isType(TOKEN_LBRACKET))
    {
        getRule(ret_params);
        getType(TOKEN_RBRACKET);
    }
    // <ret> -> ε
    else if (isType(TOKEN_LCURLYBRACKET))
    {
        parser->tokenProcessed = false;
    }
    returnRule();
}

/**
 * @brief <ret_params> rule
 * 
 * @return Error code of rule 
 */
int ret_params(Parser parser)
{
    getToken();
    //<ret_params> -> <type> <ret_params_n>

    if (isKeyword(KW_INT) || isKeyword(KW_FLOAT64) || isKeyword(KW_STRING))
    {
        if (isKeyword(KW_INT))
        {
            strAddChar(&parser->currentFunc->returnTypes, '0');
        }
        else if (isKeyword(KW_FLOAT64))
        {
            strAddChar(&parser->currentFunc->returnTypes, '1');
        }
        else if (isKeyword(KW_STRING))
        {
            strAddChar(&parser->currentFunc->returnTypes, '2');
        }
        getRule(ret_params_n);
    }
    //<ret_params>->ε
    else if (isType(TOKEN_RBRACKET))
    {
        parser->tokenProcessed = false;
    }
    returnRule();
}

/**
 * @brief <ret_params_n> rule
 * 
 * @return Error code of rule 
 */
int ret_params_n(Parser parser)
{
    getToken();
    //<ret_params_n>->, <type> <ret_params_n>
    if (isType(TOKEN_COMMA))
    {
        getToken();
        if (isKeyword(KW_INT))
        {
            strAddChar(&parser->currentFunc->returnTypes, '0');
        }
        else if (isKeyword(KW_FLOAT64))
        {
            strAddChar(&parser->currentFunc->returnTypes, '1');
        }
        else if (isKeyword(KW_STRING))
        {
            strAddChar(&parser->currentFunc->returnTypes, '2');
        }
        getRule(ret_params_n);
    }
    //<ret_params_n>->ε
    else if (isType(TOKEN_RBRACKET))
    {
        parser->tokenProcessed = false;
    }
    returnRule();
}

/**
 * @brief <body> rule
 * 
 * @return Error code of rule 
 */
int body(Parser parser)
{
    getToken();
    // <body> -> FOR <for_definition> ; <expression> ; <for_assign> { EOL <body> } EOL <body>
    if (isKeyword(KW_FOR))
    {
        getRule(for_definition);
        getType(TOKEN_SEMICOLON);
        parser->exprBoolAllowed = true;
        getRule(expression);
        parser->exprBoolAllowed = false;
        getType(TOKEN_SEMICOLON);
        getRule(for_assign);
        getType(TOKEN_LCURLYBRACKET);
        getType(TOKEN_EOL);
        getRule(body);
        getType(TOKEN_RCURLYBRACKET);
        getType(TOKEN_EOL);
        getRule(body);
    }
    // <body> -> IF <expression> { EOL <body> } ELSE { EOL <body> } EOL <body>
    else if (isKeyword(KW_IF))
    {
        parser->exprBoolAllowed = true;
        getRule(expression);
        parser->exprBoolAllowed = false;
        getType(TOKEN_LCURLYBRACKET);
        getType(TOKEN_EOL);
        getRule(body);
        getType(TOKEN_RCURLYBRACKET);
        getKeyword(KW_ELSE);
        getType(TOKEN_LCURLYBRACKET);
        getType(TOKEN_EOL);
        getRule(body);
        getType(TOKEN_RCURLYBRACKET);
        getType(TOKEN_EOL);
        getRule(body);
    }
    // <body> -> RETURN <ret_values> EOL <body>
    else if (isKeyword(KW_RETURN))
    {
        getRule(ret_values);
        getType(TOKEN_EOL);
        getRule(body);
    }
    // <body> -> ID <body_n> EOL <body>
    else if (isType(TOKEN_IDENTIFIER))
    {
        getRule(body_n);
        getType(TOKEN_EOL);
        getRule(body);
    }
    // skip empty lines
    else if (isType(TOKEN_EOL))
    {
        getRule(body);
    }
    // <body> -> ε
    else if (isType(TOKEN_RCURLYBRACKET))
    {
        parser->tokenProcessed = false;
    }
    returnRule();
}

/**
 * @brief <body_n> rule
 * 
 * @return Error code of rule 
 */
int body_n(Parser parser)
{
    getToken();
    //<body_n> -> <definition>
    if (isType(TOKEN_VAR_DEF))
    {
        parser->tokenProcessed = false;
        getRule(definition);
    }
    //<body_n> -> <assign>
    else if (isType(TOKEN_COMMA) || isType(TOKEN_ASSIGN))
    {
        parser->tokenProcessed = false;
        getRule(assign);
    }
    //<body_n>-> ( <arg> )
    else if (isType(TOKEN_LBRACKET))
    {
        getRule(arg);
        getType(TOKEN_RBRACKET);
    }
    returnRule();
}

/**
 * @brief <id_n> rule
 * 
 * @return Error code of rule 
 */
int id_n(Parser parser)
{
    getToken();
    //  <id_n> -> , ID <id_n>
    if (isType(TOKEN_COMMA))
    {
        parser->countLeft++;
        getType(TOKEN_IDENTIFIER);
        getRule(id_n);
    }
    //  <id_n> -> ε
    else if (isType(TOKEN_ASSIGN))
    {
        parser->tokenProcessed = false;
    }
    returnRule();
}

/**
 * @brief <for_definition> rule
 * 
 * @return Error code of rule 
 */
int for_definition(Parser parser)
{
    getToken();
    // <for_definition> -> ID <definition>
    if (isType(TOKEN_IDENTIFIER))
    {
        parser->countLeft = 1;
        getRule(definition);
    }
    // <for_definition> -> ε
    else if (isType(TOKEN_SEMICOLON))
    {
        parser->tokenProcessed = false;
    }
    returnRule();
}

/**
 * @brief <for_assign> rule
 * 
 * @return Error code of rule 
 */
int for_assign(Parser parser)
{
    getToken();
    // <for_assign> -> ID <assign>
    if (isType(TOKEN_IDENTIFIER))
    {
        parser->countLeft = 1;
        getRule(assign);
    }
    // <for_assign> -> ε
    else if (isType(TOKEN_LCURLYBRACKET))
    {
        parser->tokenProcessed = false;
    }
    returnRule();
}

/**
 * @brief <value> rule
 * 
 * @return Error code of rule 
 */
int value(Parser parser)
{
    parser->countRight = 1;
    // <value> -> ID <func>
    getRule(expression);
    if (parser->funcInExpr)
    {
        getRule(func);
    }
    // <value> -> <expression> <expression_n>
    else
    {
        getRule(expression_n);
    }
    return ERROR_CODE_OK;
}

/**
 * @brief <expression_n> rule
 * 
 * @return Error code of rule 
 */
int expression_n(Parser parser)
{
    getToken();
    // <expression_n> -> , <expression> <expression_n>
    if (isType(TOKEN_COMMA))
    {
        parser->countRight++;
        getRule(expression);
        getRule(expression_n);
    }
    // <expression_n> -> ε
    else if (isType(TOKEN_LCURLYBRACKET) || isType(TOKEN_EOL))
    {
        parser->tokenProcessed = false;
    }
    returnRule();
}

/**
 * @brief <definition> rule
 * 
 * @return Error code of rule 
 */
int definition(Parser parser)
{
    parser->countLeft = 1;
    // <definition> -> := <expression>
    getType(TOKEN_VAR_DEF);
    getRule(expression);
    return ERROR_CODE_OK;
}

/**
 * @brief <assign> rule
 * 
 * @return Error code of rule 
 */
int assign(Parser parser)
{
    parser->countLeft = 1;
    // <assign> -> <id_n> = <value>
    getRule(id_n);
    getType(TOKEN_ASSIGN);
    getRule(value);
    // number of items on the left and right is not equal
    if (parser->countLeft != parser->countRight)
        return ERROR_SEM_OTHER;
    return ERROR_CODE_OK;
}

/**
 * @brief <func> rule
 * 
 * @return Error code of rule 
 */
int func(Parser parser)
{
    getToken();
    // <func> -> ( <arg> )
    if (isType(TOKEN_LBRACKET))
    {
        getRule(arg);
        getType(TOKEN_RBRACKET);
    }
    // <func> -> ε
    else if (isType(TOKEN_LCURLYBRACKET) || isType(TOKEN_EOL))
    {
        parser->tokenProcessed = false;
    }
    returnRule();
}

/**
 * @brief <arg> rule
 * 
 * @return Error code of rule 
 */
int arg(Parser parser)
{
    getToken();
    // <arg> -> <term> <term_n>
    if (isType(TOKEN_IDENTIFIER) || isType(TOKEN_INT) || isType(TOKEN_FLOAT) || isType(TOKEN_STRING))
    {
        parser->tokenProcessed = false;
        getRule(term);
        getRule(term_n);
    }
    // <arg> -> ε
    else if (isType(TOKEN_RBRACKET))
    {
        parser->tokenProcessed = false;
    }
    returnRule();
}

/**
 * @brief <term> rule
 * 
 * @return Error code of rule 
 */
int term(Parser parser)
{
    // TODO - semantic
    getToken();
    // <term> -> ID
    if (isType(TOKEN_IDENTIFIER))
    {
        if (!strCmpConstStr(parser->token.attribute.string, "_"))
            return ERROR_SEM_OTHER;
    }
    // <term> -> VALUE_INT
    else if (isType(TOKEN_INT))
    {
    }
    // <term> -> VALUE_FLOAT64
    else if (isType(TOKEN_FLOAT))
    {
    }
    // <term> -> VALUE_STRING
    else if (isType(TOKEN_STRING))
    {
    }
    returnRule();
}

/**
 * @brief <term_n> rule
 * 
 * @return Error code of rule 
 */
int term_n(Parser parser)
{
    getToken();
    // <term_n> -> , <term> <term_n>
    if (isType(TOKEN_COMMA))
    {
        getRule(term);
        getRule(term_n);
    }
    // <term_n> -> ε
    else if (isType(TOKEN_RBRACKET))
    {
        parser->tokenProcessed = false;
    }
    returnRule();
}

/**
 * @brief <ret_values> rule
 * 
 * @return Error code of rule 
 */
int ret_values(Parser parser)
{
    getToken();
    //<ret_values> -> ε
    if (isType(TOKEN_EOL))
    {
        parser->countRight = 0;
        parser->tokenProcessed = false;
        return ERROR_CODE_OK;
    }
    //<ret_values> -> <value>
    else
    {
        parser->tokenProcessed = false;
        getRule(value);
        return ERROR_CODE_OK;
    }
}