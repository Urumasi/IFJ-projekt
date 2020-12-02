/**
* IFJ Project 2020: Team 008, variant II
*
* @file taclist.c
*
* @brief Three address code list implementation.
*
*
* @author Martin Kneslík <xknesl02@stud.fit.vutbr.cz>
* @author Adriana Jurkechová <xjurke02@stud.fit.vutbr.cz>
* @author Karel Norek <xnorek01@stud.fit.vutbr.cz>
* @author Petr Salaba <xsalab00@stud.fit.vutbr.cz>
*/

#include "taclist.h"
#include <malloc.h>

int tac_init(TAC_list *list) {
	list->head = malloc(sizeof(struct s_TAC_list_block));
	list->head->next = NULL;
	list->tail = list->head;
	list->length = 0;
	list->blockCount = 1;
}

TAC *tac_get_line(TAC_list *list, const unsigned int idx) {
	static TAC_list_block_ptr cached_block;
	static unsigned int cached_block_id = -1;

	if (!list || idx >= list->length)
		return NULL;

	unsigned int block_id = idx / TAC_COUNT_PER_ALLOC;
	if (block_id >= list->blockCount)
		return NULL;

	TAC_list_block_ptr block;
	if (block_id == cached_block_id) {
		block = cached_block;
	} else {
		for (unsigned int i = block_id; i; --i) {
			if (!block->next)
				return NULL;
			block = block->next;
		}
		cached_block = block;
		cached_block_id = block_id;
	}

	return &block->data[idx % TAC_COUNT_PER_ALLOC];
}

int tac_append_line(TAC_list *list, InstructionType type, string *dest, string *op1, string *op2) {
	TAC_list_block_ptr block = list->tail;

	unsigned int idx = list->length % TAC_COUNT_PER_ALLOC;
	list->length++;
	if(idx == 0){ // We've gone to a new block
		TAC_list_block_ptr new_block = malloc(sizeof(struct s_TAC_list_block));
		new_block->next = NULL;
		block->next = new_block;
		list->tail = new_block;

		block = new_block;
	}

	block->data[idx].type = type;
	strInitCopy(&block->data[idx].destination, dest);
	strInitCopy(&block->data[idx].operand1, op1);
	strInitCopy(&block->data[idx].operand2, op2);
}
