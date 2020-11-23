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
#include "scanner.h"
#include "parser.h"
#include "expression.h"
#include "error.h"

Parser parser;

int initParser(Parser *parser)
{
    parser->tokenProcessed = true;
    parser->declaredMain = false;
    parser->funcInExpr = false;
}

/**
 * @brief Main function
 * 
 * @return Error code of parser
 */
int parse()
{
    Parser parser;
    initParser(&parser);
    parser.returnCode = package(&parser);
    if (parser.returnCode != ERROR_CODE_OK)
        return parser.returnCode;

    // missing main function
    if (parser.declaredMain == false)
        return ERROR_SEM;
}

/**
 * @brief <package> rule
 * 
 * @return Error code of rule 
 */
int package(Parser *parser)
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
int prog(Parser *parser)
{
    getToken();
    // <prog> -> FUNC ID ( <params> ) <ret> { EOL <body> } EOL <prog>
    if (isKeyword(KW_FUNC))
    {
        getType(TOKEN_IDENTIFIER);
        if (!strCmpConstStr(parser->token.attribute.string, "main"))
            parser->declaredMain = true;
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
int params(Parser *parser)
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
int type(Parser *parser)
{
    // TODO - semantic
    getToken();
    if (isKeyword(KW_INT))
    {
    }
    else if (isKeyword(KW_FLOAT64))
    {
    }
    else if (isKeyword(KW_STRING))
    {
    }
    returnRule();
}

/**
 * @brief <params_n> rule
 * 
 * @return Error code of rule 
 */
int params_n(Parser *parser)
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
int ret(Parser *parser)
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
int ret_params(Parser *parser)
{
    getToken();
    //<ret_params> -> <type> <ret_params_n>
    if (isKeyword(KW_INT) || isKeyword(KW_FLOAT64) || isKeyword(KW_STRING))
    {
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
int ret_params_n(Parser *parser)
{
    getToken();
    //<ret_params_n>->, <type> <ret_params_n>
    if (isType(TOKEN_COMMA))
    {
        getRule(type);
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
int body(Parser *parser)
{
    getToken();
    // <body> -> FOR <definition> ; <expression> ; <assign> { EOL <body> } EOL <body>
    if (isKeyword(KW_FOR))
    {
        getRule(definition);
        getType(TOKEN_SEMICOLON);
        getRule(expression);
        parser->tokenProcessed = false;
        getType(TOKEN_SEMICOLON);
        getRule(assign);
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
        getRule(expression);
        parser->tokenProcessed = false;
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
    // <body> -> RETURN <list> EOL <body>
    else if (isKeyword(KW_RETURN))
    {
        getRule(list);
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
int body_n(Parser *parser)
{
    getToken();
    //<body_n>-> := <expression>
    if (isType(TOKEN_VAR_DEF))
    {
        getRule(expression);
    }
    //<body_n>-> <id_n> = <value>
    else if (isType(TOKEN_COMMA) || isType(TOKEN_ASSIGN))
    {
        parser->tokenProcessed = false;
        getRule(id_n);
        getType(TOKEN_ASSIGN);
        getRule(value);
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
int id_n(Parser *parser)
{
    getToken();
    //  <id_n> -> , ID <id_n>
    if (isType(TOKEN_COMMA))
    {
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
 * @brief <definition> rule
 * 
 * @return Error code of rule 
 */
int definition(Parser *parser)
{
    getToken();
    // <definition> -> ID := <expression>
    if (isType(TOKEN_IDENTIFIER))
    {
        getType(TOKEN_VAR_DEF);
        getRule(expression);
    }
    // <definition> -> ε
    else if (isType(TOKEN_SEMICOLON))
    {
        parser->tokenProcessed = false;
    }
    returnRule();
}

/**
 * @brief <assign> rule
 * 
 * @return Error code of rule 
 */
int assign(Parser *parser)
{
    getToken();
    // <assign> -> ID <id_n> = <value>
    if (isType(TOKEN_IDENTIFIER))
    {
        getRule(id_n);
        getType(TOKEN_ASSIGN);
        getRule(value);
    }
    // <assign> -> ε
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
int value(Parser *parser)
{
    // <value>->ID<func>
    getRule(expression);
    if (parser->funcInExpr)
    {
        getRule(func);
    }
    // <value>-><expression><expression_n>
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
int expression_n(Parser *parser)
{
    getToken();
    // <expression_n> -> , <expression> <expression_n>
    if (isType(TOKEN_COMMA))
    {
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
 * @brief <func> rule
 * 
 * @return Error code of rule 
 */
int func(Parser *parser)
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
int arg(Parser *parser)
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
int term(Parser *parser)
{
    // TODO - semantic
    getToken();
    // <term> -> ID
    if (isType(TOKEN_IDENTIFIER))
    {
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
int term_n(Parser *parser)
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
 * @brief <list> rule
 * 
 * @return Error code of rule 
 */
int list(Parser *parser)
{
    getToken();
    //<list> -> ε
    if (isType(TOKEN_EOL))
    {
        parser->tokenProcessed = false;
        return ERROR_CODE_OK;
    }
    //<list> -> <value>
    else
    {
        parser->tokenProcessed = false;
        getRule(value);
        return ERROR_CODE_OK;
    }
}