/**
 * IFJ Project 2020: Team 008, variant II
 *
 * @file generator.c
 *
 * @brief Code generator implementation.
 *
 *
 * @author Martin Kneslík <xknesl02@stud.fit.vutbr.cz>
 * @author Adriana Jurkechová <xjurke02@stud.fit.vutbr.cz>
 * @author Karel Norek <xnorek01@stud.fit.vutbr.cz>
 * @author Petr Salaba <xsalab00@stud.fit.vutbr.cz>
 */

#include <stdio.h>
#include "generator.h"
#include "str.h"

unsigned int label_counter = 0;

void sanitize_string(string *in, string *out) {
    strClear(out);
    char buffer[5];
    for (unsigned int i = 0; in->str[i]; ++i) {
        char c = in->str[i];
        if (c <= ' ' || c == '\\') {
            snprintf(buffer, 5, "\\%3d", (int) c);
            for (unsigned int j = 0; buffer[j]; ++j) {
                if (buffer[j] == ' ')
                    strAddChar(out, '0');
                else
                    strAddChar(out, buffer[j]);
            }
        } else {
            strAddChar(out, c);
        }
    }
}

void generate_prog_init() {
    printf(".IFJcode20\n"
           "DEFVAR GF@$printvar\n"
           "CALL func$main\n"
           "DEFVAR GF@$rettype\n"
           "TYPE GF@$rettype TF@$retval\n"
           "JUMPIFNEQ $return0 GF@$rettype string@int\n"
           "EXIT TF@$retval\n"
           "LABEL $return0\n"
           "EXIT int@0\n");
}

void generate_func_header(Token *token) {
    printf("LABEL func$%s\n"
           "CREATEFRAME\n"
           "PUSHFRAME\n"
           "DEFVAR LF@$retval\n", token->attribute.string->str);
}

void generate_func_end() {
    printf("POPFRAME\n"
           "RETURN\n");
}

void generate_func_call(string *func_name) {
    if(!strCmpConstStr(func_name, "print")){
        printf("POPS GF@$printvar\n"
               "WRITE GF@$printvar\n");
    }else {
        printf("CALL func$%s\n", func_name->str);
    }
}

int generate_if_then() {
    printf("PUSHS bool@true\n"
           "JUMPIFNEQS else$%u\n", label_counter);
    return label_counter++;
}

void generate_if_else(unsigned int label) {
    printf("JUMP endif$%u\n"
           "LABEL else$%u\n", label, label);
}

void generate_if_end(unsigned int label) {
    printf("LABEL endif$%u\n", label);
}

void generate_var_definition(Token *token) {
    printf("DEFVAR LF@%s\n", token->attribute.string->str);
}

void generate_var_definition_assign(string *var_name) {
    printf("POPS LF@%s\n", var_name->str);
}

void generate_push(Token *token) {
    string sanitized_string;
    switch (token->type) {
        case TOKEN_INT:
            printf("PUSHS int@%d\n", token->attribute.integer);
            break;
        case TOKEN_FLOAT:
            printf("PUSHS float@%a\n", token->attribute.decimal);
            break;
        case TOKEN_STRING:
            strInit(&sanitized_string);
            sanitize_string(token->attribute.string, &sanitized_string);
            printf("PUSHS string@%s\n", sanitized_string.str);
            strFree(&sanitized_string);
            break;
        case TOKEN_IDENTIFIER:
            printf("PUSHS LF@%s\n", token->attribute.string->str);
            break;
        default:
            break;
    }
}

void generate_binary_op(Prec_symbol symbol) {
    switch (symbol) {
        case PLUS:
            printf("ADDS\n");
            break;
        case MINUS:
            printf("SUBS\n");
            break;
        case MULTIPLY:
            printf("MULS\n");
            break;
        case DIVIDE:
            printf("DIVS\n");
            break;
        default:
            break;
    }
}

void generate_comparison(Prec_symbol symbol) {
    switch (symbol) {
        case LESSER:
            printf("LTS\n");
            break;
        case GRT_OR_EQ:
            printf("LTS\n"
                   "NOTS\n");
            break;
        case GREATER:
            printf("GTS\n");
            break;
        case LESS_OR_EQ:
            printf("GTS\n"
                   "NOTS\n");
            break;
        case EQUAL:
            printf("EQS\n");
            break;
        case NOT_EQUAL:
            printf("EQS\n"
                   "NOTS\n");
            break;
        default:
            break;
    }
}

void generate_return() {
    printf("POPS LF@$retval\n"
           "POPFRAME\n"
           "RETURN\n");
}
