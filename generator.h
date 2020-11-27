/**
 * IFJ Project 2020: Team 008, variant II
 *
 * @file generator.h
 *
 * @brief Code generator.
 *
 *
 * @author Martin Kneslík <xknesl02@stud.fit.vutbr.cz>
 * @author Adriana Jurkechová <xjurke02@stud.fit.vutbr.cz>
 * @author Karel Norek <xnorek01@stud.fit.vutbr.cz>
 * @author Petr Salaba <xsalab00@stud.fit.vutbr.cz>
 */

#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include "scanner.h"
#include "expression.h"

void generate_prog_init();

void generate_func_header(Token *token);
void generate_func_end();
void generate_func_call(string *func_name);

int generate_if_then();
void generate_if_else(unsigned int label);
void generate_if_end(unsigned int label);

void generate_var_definition(Token *token);
void generate_var_definition_assign(string *var_name);

void generate_push(Token *token);
void generate_binary_op(Prec_symbol symbol);
void generate_comparison(Prec_symbol symbol);

void generate_return();

#endif //__GENERATOR_H__
