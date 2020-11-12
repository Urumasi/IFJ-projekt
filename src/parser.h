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

int parse();

int package();
int prog();
int params();
int params_n();
int ret();
int ret_params();
int ret_params_n();
int type();
int body();
int body_n();
int id_n();
int definition();
int assign();
int value();
int value_n();
int func();
int arg();

#endif