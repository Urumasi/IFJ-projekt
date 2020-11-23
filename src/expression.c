/**
 * IFJ Project 2020: Team 008, variant II
 * 
 * @file expression.c
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

/*
TODO

syntaktická kontrola výrazů
-nesmí být prázdný výraz
-nesmí se vyskytovat volání funkce
-volání funkce může být jen na začátku a nesmí pokračovat výrazem ( a, b = func() ) - domluvíme se potom
Příklad:
    x := fn() + 5 - syntaktická chyba, pokud nemáš FUNEXP
    x := fn() - syntaktická chyba, pokud nemáš MULTIVAL 
    x = fn() + 5 - syntaktická chyba, pokud nemáš FUNEXP
    x = fn() - vždy syntakticky správně

sémantika - možná budu dělat já
-kontrola dělení 0
-kontrola typů
-kontrola návratové hodnoty
    - v if/for to musí vracet BOOL, takže tam musí být porovnávání
    - v přiřazení/dekleraci porovnávání být nesmí
*/

int expression(Parser *parser)
{
    parser->funcInExpr = false;
    bool idFirst = false;
    int tokenCount = 0;
    do
    {
        getToken();

        if (tokenCount == 0 && isType(TOKEN_IDENTIFIER))
            idFirst = true;
        else if (tokenCount == 1 && idFirst && isType(TOKEN_LBRACKET))
        {
            parser->funcInExpr = true;
            parser->tokenProcessed = false;
            return ERROR_CODE_OK;
        }

        tokenCount++;
    } while (!(isType(TOKEN_SEMICOLON) || isType(TOKEN_COMMA) || isType(TOKEN_LCURLYBRACKET) || isType(TOKEN_EOL) || isType(TOKEN_RBRACKET)));
    parser->tokenProcessed = false;
    return (tokenCount == 1) ? ERROR_SYN : ERROR_CODE_OK;
}