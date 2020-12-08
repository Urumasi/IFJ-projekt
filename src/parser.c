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
#include "generator.h"

Parser parser;

int addBuiltinFunc(char *key, char *argumentTypes, char *returnTypes, Parser parser)
{
    symInsertGlobal(key);
    for (int i = 0; i < strlen(argumentTypes); i++)
        strAddChar(&parser->currentFunc->argumentTypes, argumentTypes[i]);
    for (int i = 0; i < strlen(returnTypes); i++)
        strAddChar(&parser->currentFunc->returnTypes, returnTypes[i]);
}

DataType keywordToType(Keyword keyword)
{
    if (keyword == KW_INT)
        return tINT;
    else if (keyword == KW_FLOAT64)
        return tFLOAT64;
    else if (keyword == KW_STRING)
        return tSTRING;
    else
        return tNONE;
}

char typeToChar(DataType type)
{
    if (type == tINT)
        return '0';
    else if (type == tFLOAT64)
        return '1';
    else if (type == tSTRING)
        return '2';
    else if (type == tNONE)
        return '3';
    else if (type == tMULTIPLE)
        return '4';
}

bool compareTypes(string typesLeft, string typesRight)
{
    if (typesLeft.str[0] == typeToChar(tMULTIPLE))
        return true;
    if (typesLeft.length != typesRight.length)
        return false;
    for (int i = 0; i < typesLeft.length; i++)
    {
        if (typesLeft.str[i] == '3')
            continue;
        else if (typesLeft.str[i] != typesRight.str[i])
            return false;
    }
    return true;
}

void addType(char type, string *typesList, tSymtableData data, Parser parser)
{
    if (data->typesSet)
    {
        if (parser->typeCounter >= typesList->length)
            parser->returnCode = ERROR_SEM_PARAM;
        if (typesList->str[parser->typeCounter] != type)
            parser->returnCode = ERROR_SEM_PARAM;
    }
    else
    {
        strAddChar(typesList, type);
        parser->returnCode = ERROR_CODE_OK;
    }
    parser->typeCounter++;
}

Parser initParser()
{
    Parser parser = malloc(sizeof(struct parser));
    strInit(&parser->id);
    strInit(&parser->typesLeft);
    strInit(&parser->typesRight);
    parser->tokenProcessed = true;
    parser->funcInExpr = false;
    parser->exprType = tNONE;
    parser->exprIsBool = false;
    parser->exprBoolAllowed = false;
    parser->idType = tNONE;
    parser->exprType = tNONE;
    symtableInit(&parser->sGlobal);
    symStackInit(&parser->sLocal);
    addBuiltinFunc("print", "4", "", parser);
    addBuiltinFunc("inputs", "", "20", parser);
    addBuiltinFunc("inputi", "", "00", parser);
    addBuiltinFunc("inputf", "", "10", parser);
    addBuiltinFunc("int2float", "0", "1", parser);
    addBuiltinFunc("float2int", "1", "0", parser);
    addBuiltinFunc("len", "2", "0", parser);
    addBuiltinFunc("substr", "200", "20", parser);
    addBuiltinFunc("ord", "20", "00", parser);
    addBuiltinFunc("chr", "0", "20", parser);

    return parser;
}

void deleteParser(Parser parser)
{
    strFree(&parser->id);
    strFree(&parser->typesLeft);
    strFree(&parser->typesRight);
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
    generate_code(prog_init);
    parser->returnCode = package(parser);
    if (parser->returnCode == ERROR_CODE_OK)
    {
        symReadGlobal("main");
        if (parser->currentFunc->argumentTypes.length != 0 || parser->currentFunc->returnTypes.length != 0)
        {
            parser->returnCode = ERROR_SEM_PARAM;
        }
    }
    int returnCode = parser->returnCode;
    deleteParser(parser);
    //print_taclist(); // TODO: remove debug
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
    generate_code(prog_end);
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
        symStackPush(&parser->sLocal);
        getType(TOKEN_IDENTIFIER);
        symInsertGlobal(parser->token.attribute.string->str);
        getType(TOKEN_LBRACKET);
        getRule(params);
        getType(TOKEN_RBRACKET);
        getRule(ret);
        parser->currentFunc->typesSet = true;
        generate_code(func);
        getType(TOKEN_LCURLYBRACKET);
        getType(TOKEN_EOL);
        getRule(body);
        getType(TOKEN_RCURLYBRACKET);
        getType(TOKEN_EOL);
        // TODO - return can be in if/else
        // if (parser->missingReturn)
        //     return ERROR_SEM_PARAM;
        symStackPop(&parser->sLocal);
        generate_code(func_end);
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
        strCopyString(&parser->id, parser->token.attribute.string);
        symInsertLocal(parser->id.str);
        parser->typeCounter = 0;
        getRule(type);
        generate_code(func_param);
        getRule(params_n);
        if (parser->typeCounter != parser->currentFunc->argumentTypes.length)
            return ERROR_SEM_PARAM;
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
    if (isKeyword(KW_INT) || isKeyword(KW_FLOAT64) || isKeyword(KW_STRING))
    {
        parser->currentID->type = keywordToType(parser->token.attribute.keyword);
        addType(typeToChar(parser->currentID->type), &parser->currentFunc->argumentTypes, parser->currentFunc, parser);
        checkReturn();
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
        strCopyString(&parser->id, parser->token.attribute.string);
        symInsertLocal(parser->id.str);
        getRule(type);
        generate_code(func_param);
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
    parser->missingReturn = true;
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
        parser->missingReturn = false;
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
        parser->typeCounter = 0;
        addType(typeToChar(keywordToType(parser->token.attribute.keyword)), &parser->currentFunc->returnTypes, parser->currentFunc, parser);
        checkReturn();
        getRule(ret_params_n);
        if (parser->typeCounter != parser->currentFunc->returnTypes.length)
            return ERROR_SEM_PARAM;
    }
    //<ret_params>->ε
    else if (isType(TOKEN_RBRACKET))
    {
        parser->missingReturn = false;
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
        if (isKeyword(KW_INT) || isKeyword(KW_FLOAT64) || isKeyword(KW_STRING))
        {
            addType(typeToChar(keywordToType(parser->token.attribute.keyword)), &parser->currentFunc->returnTypes, parser->currentFunc, parser);
            checkReturn();
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
    strClear(&parser->typesLeft);
    strClear(&parser->typesRight);
    getToken();
    // <body> -> FOR <for_definition> ; <expression> ; <for_assign> { EOL <body> } EOL <body>
    if (isKeyword(KW_FOR))
    {
        symStackPush(&parser->sLocal);
        generate_code(for_begin);
        getRule(for_definition);
        getType(TOKEN_SEMICOLON);
        generate_code(for_condition);
        parser->exprBoolAllowed = true;
        getRule(expression);
        parser->exprBoolAllowed = false;
        getType(TOKEN_SEMICOLON);
        generate_code(for_assign);
        getRule(for_assign);
        getType(TOKEN_LCURLYBRACKET);
        getType(TOKEN_EOL);
        symStackPush(&parser->sLocal);
        generate_code(for);
        getRule(body);
        getType(TOKEN_RCURLYBRACKET);
        getType(TOKEN_EOL);
        symStackPop(&parser->sLocal);
        symStackPop(&parser->sLocal);
        generate_code(endfor);
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
        generate_code(if);
        symStackPush(&parser->sLocal);
        getRule(body);
        symStackPop(&parser->sLocal);
        getType(TOKEN_RCURLYBRACKET);
        getKeyword(KW_ELSE);
        getType(TOKEN_LCURLYBRACKET);
        getType(TOKEN_EOL);
        generate_code(else);
        symStackPush(&parser->sLocal);
        getRule(body);
        symStackPop(&parser->sLocal);
        getType(TOKEN_RCURLYBRACKET);
        getType(TOKEN_EOL);
        generate_code(endif);
        getRule(body);
    }
    // <body> -> RETURN <ret_values> EOL <body>
    else if (isKeyword(KW_RETURN))
    {
        if (parser->sLocal.scopeCount == 1)
            parser->missingReturn = false;
        parser->typeCounter = 0;
        getRule(ret_values);
        if (!compareTypes(parser->typesRight, parser->currentFunc->returnTypes))
            return ERROR_SEM_PARAM;
        getType(TOKEN_EOL);
        generate_code(return);
        getRule(body);
    }
    // <body> -> ID <body_n> EOL <body>
    else if (isType(TOKEN_IDENTIFIER))
    {
        strCopyString(&parser->id, parser->token.attribute.string);
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
        parser->tokenProcessed = false;
        getRule(func);
        generate_code(void_func_call);
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
        getType(TOKEN_IDENTIFIER);
        strCopyString(&parser->id, parser->token.attribute.string);
        if (strCmpConstStr(&parser->id, "_") != 0)
        {
            symReadLocal(parser->id.str);
            parser->idType = parser->currentID->type;
        }
        else
        {
            parser->idType = tNONE;
        }
        generate_code(assign_push_id);
        strAddChar(&parser->typesLeft, typeToChar(parser->idType));
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
        strCopyString(&parser->id, parser->token.attribute.string);
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
        strCopyString(&parser->id, parser->token.attribute.string);
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
    // <value> -> ID <func>
    getRule(expression);
    if (parser->funcInExpr)
    {
        getType(TOKEN_IDENTIFIER);
        strCopyString(&parser->id, parser->token.attribute.string);
        getToken();
        if (!isType(TOKEN_LBRACKET))
            return ERROR_SEM;
        parser->tokenProcessed = false;
        getRule(func);
        generate_code(func_call);
    }
    // <value> -> <expression> <expression_n>
    else
    {
        strAddChar(&parser->typesRight, typeToChar(parser->exprType));
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
        getRule(expression);
        strAddChar(&parser->typesRight, typeToChar(parser->exprType));
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
    // <definition> -> := <expression>
    getType(TOKEN_VAR_DEF);
    getRule(expression);
    if (parser->funcInExpr)
        return ERROR_SEM;
    symInsertLocal(parser->id.str);
    parser->currentID->type = parser->exprType;
    generate_code(define_var);
    return ERROR_CODE_OK;
}

/**
 * @brief <assign> rule
 * 
 * @return Error code of rule 
 */
int assign(Parser parser)
{
    if (strCmpConstStr(&parser->id, "_") != 0)
    {
        symReadLocal(parser->id.str);
        parser->idType = parser->currentID->type;
    }
    else
    {
        parser->idType = tNONE;
    }
    generate_code(assign_push_id);
    strAddChar(&parser->typesLeft, typeToChar(parser->idType));
    // <assign> -> <id_n> = <value>
    getRule(id_n);
    getType(TOKEN_ASSIGN);
    getRule(value);
    if (!compareTypes(parser->typesLeft, parser->typesRight))
        return ERROR_SEM_OTHER;
    generate_code(assign);
    checkReturn();
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
        if (symtableReadStack(&parser->sLocal, parser->id.str) != NULL)
            return ERROR_SEM;
        parser->calledFunc = symtableInsert(&parser->sGlobal, parser->id.str);
        symCheckNull(parser->calledFunc);
        getRule(arg);
        getType(TOKEN_RBRACKET);
        if (!parser->calledFunc->typesSet && !parser->calledFunc->defined)
        {
            strCopyString(&parser->calledFunc->argumentTypes, &parser->typesRight);
            strCopyString(&parser->calledFunc->returnTypes, &parser->typesLeft);
            parser->calledFunc->typesSet = true;
        }
        if (!compareTypes(parser->calledFunc->argumentTypes, parser->typesRight))
            return ERROR_SEM_PARAM;
        strCopyString(&parser->typesRight, &parser->calledFunc->returnTypes);
    }
    // <func> -> ε
    // else if (isType(TOKEN_LCURLYBRACKET) || isType(TOKEN_EOL))
    // {
    //     parser->tokenProcessed = false;
    // }
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
        generate_code(stack_push);
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
        strCopyString(&parser->id, parser->token.attribute.string);
        symReadLocal(parser->token.attribute.string->str);
        strAddChar(&parser->typesRight, typeToChar(parser->currentID->type));
    }
    // <term> -> VALUE_INT
    else if (isType(TOKEN_INT))
    {
        strAddChar(&parser->typesRight, typeToChar(tINT));
    }
    // <term> -> VALUE_FLOAT64
    else if (isType(TOKEN_FLOAT))
    {
        strAddChar(&parser->typesRight, typeToChar(tFLOAT64));
    }
    // <term> -> VALUE_STRING
    else if (isType(TOKEN_STRING))
    {
        strAddChar(&parser->typesRight, typeToChar(tSTRING));
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
        generate_code(stack_push);
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
