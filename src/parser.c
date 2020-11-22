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

/**
 * @brief Returns syntax error code
 * 
 */
#define syntaxError() return ERROR_SYN

/**
 * @brief Gets new token if token isn't processed. Returns error code if anything fails.
 *        Unprocessed token occurs in "<rule> -> eps" rule, meaning that current token 
 *        is from another rule and in next getToken() call, token remains the same
 * 
 */
#define getToken()                                               \
    if (tokenProcesed)                                           \
    {                                                            \
        if ((tokenCode = getNextToken(&token)) != ERROR_CODE_OK) \
            return tokenCode;                                    \
    }                                                            \
    else                                                         \
        tokenProcesed = true

/**
 * @brief Checks if token type is same as argument
 * 
 */
#define isType(_token) (token.type == _token)

/**
 * @brief Gets new token and checks type. Returns syntax error code, if types don't match
 * 
 */
#define getType(_token)  \
    getToken();          \
    if (!isType(_token)) \
    syntaxError()

/**
 * @brief Checks if token type is keyword and then checks if token attribute is same as argument
 * 
 */
#define isKeyword(_keyword) ((token.type == TOKEN_KEYWORD) && (token.attribute.keyword == _keyword))

/**
 * @brief Gets new token, checks if token type is keyword ane then checks token attribute. Returns syntax error code, if keywords don't match
 * 
 */
#define getKeyword(_keyword)                 \
    getType(TOKEN_KEYWORD);                  \
    if (token.attribute.keyword != _keyword) \
    syntaxError()

/**
 * @brief Calls function in argument. If function doesn't return 0, returns that value.
 * 
 */
#define getRule(_rule)          \
    error = _rule();            \
    if (error != ERROR_CODE_OK) \
    return error

//Global variables
Token token;
int tokenCode;
bool tokenProcesed = true;

/**
 * @brief Main function
 * 
 * @return Error code of parser
 */
int parse()
{
    int error = package();
    return error;
}

/**
 * @brief <package> rule
 * 
 * @return Error code of rule 
 */
int package()
{
    getToken();
    while (isType(TOKEN_EOL))
        getToken();
    tokenProcesed = false;
    // <package> -> PACKAGE ID EOL <prog>
    getKeyword(KW_PACKAGE);
    getType(TOKEN_IDENTIFIER);
    getType(TOKEN_EOL);
    return prog();
}

/**
 * @brief <prog> rule
 * 
 * @return Error code of rule 
 */
int prog()
{
    int error;
    getToken();
    // <prog> -> FUNC ID ( <params> ) <ret> { EOL <body> } EOL <prog>
    if (isKeyword(KW_FUNC))
    {
        getType(TOKEN_IDENTIFIER);
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
        return ERROR_CODE_OK;
    }
    //<prog> -> EOF
    else if (isType(TOKEN_EOF))
    {
        return ERROR_CODE_OK;
    }
    else if (isType(TOKEN_EOL))
    {
        getRule(prog);
        return ERROR_CODE_OK;
    }
    syntaxError();
}

/**
 * @brief <params> rule
 * 
 * @return Error code of rule 
 */
int params()
{
    int error;
    getToken();
    // <params> -> ID <type> <params_n>
    if (isType(TOKEN_IDENTIFIER))
    {
        getRule(type);
        getRule(params_n);
        return ERROR_CODE_OK;
    }
    // <params> -> ε
    else if (isType(TOKEN_RBRACKET))
    {
        tokenProcesed = false;
        return ERROR_CODE_OK;
    }
    syntaxError();
}

/**
 * @brief <type> rule
 * 
 * @return Error code of rule 
 */
int type()
{
    getToken();
    if (isKeyword(KW_INT) || isKeyword(KW_FLOAT64) || isKeyword(KW_STRING))
        return ERROR_CODE_OK;
    syntaxError();
}

/**
 * @brief <params_n> rule
 * 
 * @return Error code of rule 
 */
int params_n()
{
    int error;
    getToken();
    // <params_n> , ID <type> <params_n>
    if (isType(TOKEN_COMMA))
    {
        getType(TOKEN_IDENTIFIER);
        getRule(type);
        getRule(params_n);
        return ERROR_CODE_OK;
    }
    // <params_n> -> ε
    else if (isType(TOKEN_RBRACKET))
    {
        tokenProcesed = false;
        return ERROR_CODE_OK;
    }
    syntaxError();
}

/**
 * @brief <ret> rule
 * 
 * @return Error code of rule 
 */
int ret()
{
    int error;
    getToken();
    // <ret> -> ( <ret_params> )
    if (isType(TOKEN_LBRACKET))
    {
        getRule(ret_params);
        getType(TOKEN_RBRACKET);
        return ERROR_CODE_OK;
    }
    // <ret> -> ε
    else if (isType(TOKEN_LCURLYBRACKET))
    {
        tokenProcesed = false;
        return ERROR_CODE_OK;
    }
    syntaxError();
}

/**
 * @brief <ret_params> rule
 * 
 * @return Error code of rule 
 */
int ret_params()
{
    int error;
    getToken();
    //<ret_params> -> <type> <ret_params_n>
    if (isKeyword(KW_INT) || isKeyword(KW_FLOAT64) || isKeyword(KW_STRING))
    {
        getRule(ret_params_n);
        return ERROR_CODE_OK;
    }
    //<ret_params>->ε
    else if (isType(TOKEN_RBRACKET))
    {
        tokenProcesed = false;
        return ERROR_CODE_OK;
    }
    syntaxError();
}

/**
 * @brief <ret_params_n> rule
 * 
 * @return Error code of rule 
 */
int ret_params_n()
{
    int error;
    getToken();
    //<ret_params_n>->, <type> <ret_params_n>
    if (isType(TOKEN_COMMA))
    {
        getRule(type);
        getRule(ret_params_n);
        return ERROR_CODE_OK;
    }
    //<ret_params_n>->ε
    else if (isType(TOKEN_RBRACKET))
    {
        tokenProcesed = false;
        return ERROR_CODE_OK;
    }
    syntaxError();
}

/**
 * @brief <body> rule
 * 
 * @return Error code of rule 
 */
int body()
{
    int error;
    getToken();
    // <body> -> FOR <definition> ; <expression> ; <assign> { EOL <body> } EOL <body>
    if (isKeyword(KW_FOR))
    {
        getRule(definition);
        getType(TOKEN_SEMICOLON);
        // getRule(expression);
        getRule(expression);
        tokenProcesed = false;
        getType(TOKEN_SEMICOLON);
        getRule(assign);
        getType(TOKEN_LCURLYBRACKET);
        getType(TOKEN_EOL);
        getRule(body);
        getType(TOKEN_RCURLYBRACKET);
        getType(TOKEN_EOL);
        getRule(body);
        return ERROR_CODE_OK;
    }
    // <body> -> IF <expression> { EOL <body> } ELSE { EOL <body> } EOL <body>
    else if (isKeyword(KW_IF))
    {
        getRule(expression);
        tokenProcesed = false;
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
        return ERROR_CODE_OK;
    }
    // <body> -> RETURN <return> EOL <body>
    else if (isKeyword(KW_RETURN))
    {
        getRule(arg);
        getType(TOKEN_EOL);
        getRule(body);
        return ERROR_CODE_OK;
    }
    // <body> -> ID <body_n> EOL <body>
    else if (isType(TOKEN_IDENTIFIER))
    {
        getRule(body_n);
        getType(TOKEN_EOL);
        getRule(body);
        return ERROR_CODE_OK;
    }
    // <body> -> ε
    else if (isType(TOKEN_EOL))
    {
        getRule(body);
        return ERROR_CODE_OK;
    }
    else if (isType(TOKEN_RCURLYBRACKET))
    {
        tokenProcesed = false;
        return ERROR_CODE_OK;
    }
    syntaxError();
}

/**
 * @brief <body_n> rule
 * 
 * @return Error code of rule 
 */
int body_n()
{
    int error;
    getToken();
    //<body_n>-> := <value>
    if (isType(TOKEN_VAR_DEF))
    {
        getRule(value);
        return ERROR_CODE_OK;
    }
    //<body_n>-> <id_n> = <value> <value_n>
    else if (isType(TOKEN_COMMA) || isType(TOKEN_ASSIGN))
    {
        tokenProcesed = false;
        getRule(id_n);
        getType(TOKEN_ASSIGN);
        getRule(value);
        getRule(value_n);
        return ERROR_CODE_OK;
    }
    //<body_n>-> ( <arg> )
    else if (isType(TOKEN_LBRACKET))
    {
        getRule(arg);
        getType(TOKEN_RBRACKET);
        return ERROR_CODE_OK;
    }
    syntaxError();
}

/**
 * @brief <id_n> rule
 * 
 * @return Error code of rule 
 */
int id_n()
{
    int error;
    getToken();
    //  <id_n> -> , ID <id_n>
    if (isType(TOKEN_COMMA))
    {
        getType(TOKEN_IDENTIFIER);
        getRule(id_n);
        return ERROR_CODE_OK;
    }
    //  <id_n> -> ε
    else if (isType(TOKEN_ASSIGN))
    {
        tokenProcesed = false;
        return ERROR_CODE_OK;
    }
    syntaxError();
}

/**
 * @brief <definition> rule
 * 
 * @return Error code of rule 
 */
int definition()
{
    int error;
    getToken();
    // <definition> -> ID := <value>
    if (isType(TOKEN_IDENTIFIER))
    {
        getType(TOKEN_VAR_DEF);
        getRule(value);
        return ERROR_CODE_OK;
    }
    // <definition> -> ε
    else if (isType(TOKEN_SEMICOLON))
    {
        tokenProcesed = false;
        return ERROR_CODE_OK;
    }
    syntaxError();
}

/**
 * @brief <assign> rule
 * 
 * @return Error code of rule 
 */
int assign()
{
    int error;
    getToken();
    // <assign> -> ID <id_n> = <value> <value_n>
    if (isType(TOKEN_IDENTIFIER))
    {
        getRule(id_n);
        getType(TOKEN_ASSIGN);
        getRule(value);
        getRule(value_n);
        return ERROR_CODE_OK;
    }
    // <assign> -> ε
    else if (isType(TOKEN_LCURLYBRACKET))
    {
        tokenProcesed = false;
        return ERROR_CODE_OK;
    }
    syntaxError();
}

/**
 * @brief <value> rule
 * 
 * @return Error code of rule 
 */
int value()
{
    int error;
    getRule(expression);
    tokenProcesed = false;
    // getToken();
    // // <value> -> ID <func>
    // if (isType(TOKEN_IDENTIFIER))
    // {
    //     getRule(func);
    //     return ERROR_CODE_OK;
    // }
    // // <value> -> VALUE_INT
    // else if (isType(TOKEN_INT))
    // {
    //     return ERROR_CODE_OK;
    // }
    // // <value> -> VALUE_FLOAT64
    // else if (isType(TOKEN_FLOAT))
    // {
    //     return ERROR_CODE_OK;
    // }
    // // <value> -> VALUE_STRING
    // else if (isType(TOKEN_STRING))
    // {
    //     return ERROR_CODE_OK;
    // }
    // // <value> -> <expression>
    // // else if ()
    // // {
    // // }
    // syntaxError();
}

/**
 * @brief <value_n> rule
 * 
 * @return Error code of rule 
 */
int value_n()
{
    int error;
    getToken();
    // <value_n> -> , <value> <value_n>
    if (isType(TOKEN_COMMA))
    {
        getRule(value);
        getRule(value_n);
        return ERROR_CODE_OK;
    }
    // <value_n> -> ε
    else if (isType(TOKEN_RBRACKET) || isType(TOKEN_LCURLYBRACKET) || isType(TOKEN_EOL))
    {
        tokenProcesed = false;
        return ERROR_CODE_OK;
    }
    syntaxError();
}

/**
 * @brief <func> rule
 * 
 * @return Error code of rule 
 */
int func()
{
    int error;
    getToken();
    // <func> -> ( <arg> )
    if (isType(TOKEN_LBRACKET))
    {
        getRule(arg);
        getType(TOKEN_RBRACKET);
        return ERROR_CODE_OK;
    }
    // <func> -> ε
    else if (isType(TOKEN_RBRACKET) || isType(TOKEN_LCURLYBRACKET) ||
             isType(TOKEN_RCURLYBRACKET) || isType(TOKEN_COMMA) ||
             isType(TOKEN_SEMICOLON) || isType(TOKEN_EOL))
    {
        tokenProcesed = false;
        return ERROR_CODE_OK;
    }
    syntaxError();
}

/**
 * @brief <arg> rule
 * 
 * @return Error code of rule 
 */
int arg()
{
    int error;
    getToken();
    // <arg> -> <value> <value_n>
    if (isType(TOKEN_IDENTIFIER) || isType(TOKEN_INT) || isType(TOKEN_FLOAT) || isType(TOKEN_STRING) || isType(TOKEN_EOL))
    {
        tokenProcesed = false;
        getRule(value);
        getRule(value_n);
        return ERROR_CODE_OK;
    }
    // <arg> -> ε
    else if (isType(TOKEN_RBRACKET) || isType(TOKEN_EOL))
    {
        tokenProcesed = false;
        return ERROR_CODE_OK;
    }
    syntaxError();
}
