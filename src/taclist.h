/**
* IFJ Project 2020: Team 008, variant II
*
* @file taclist.h
*
* @brief Three address code list.
*
*
* @author Martin Kneslík <xknesl02@stud.fit.vutbr.cz>
* @author Adriana Jurkechová <xjurke02@stud.fit.vutbr.cz>
* @author Karel Norek <xnorek01@stud.fit.vutbr.cz>
* @author Petr Salaba <xsalab00@stud.fit.vutbr.cz>
*/

#ifndef __TACLIST_H__
#define __TACLIST_H__

#include "generator.h"
#include "str.h"

#define TAC_COUNT_PER_ALLOC 16

typedef struct s_TAC_list_block {
	TAC data[TAC_COUNT_PER_ALLOC];
	struct s_TAC_list_block *next;
} *TAC_list_block_ptr;

typedef struct s_TAC_list { // Three address code
	TAC_list_block_ptr head;
	TAC_list_block_ptr tail; // For appending
	unsigned int length;
	unsigned int blockCount;
} TAC_list;

int tac_init(TAC_list *list);

TAC *tac_get_line(TAC_list *list, const unsigned int idx);

int tac_append_line(TAC_list *list, InstructionType type, string *dest, string *op1, string *op2);

#endif //__TACLIST_H__
