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

int tac_list_init(TAC_list *list) {
    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
    list->blockCount = 0;
    return 0;
}

void tac_list_free(TAC_list *list) {
    if (!list->head)
        return;
    TAC_list_block_ptr block = list->head;
    TAC_list_block_ptr next_block;
    do {
        next_block = block->next;
        unsigned int free_count;
        if (next_block) { // This block is full
            free_count = TAC_COUNT_PER_ALLOC;
        } else { // Must account for uninitialized values
            free_count = list->length % TAC_COUNT_PER_ALLOC;
        }
        for (unsigned int i = 0; i < free_count; ++i) {
            tac_free(block->data[i]);
            free(block->data[i]);
        }
        free(block);
        block = next_block;
    } while (next_block);
}

int tac_list_append_new_block(TAC_list *list) {
    if (!list)
        return 1;
    TAC_list_block_ptr new_block = malloc(sizeof(struct s_TAC_list_block));
    if (!new_block)
        return 1;
    new_block->next = NULL;
    if (!list->head)
        list->head = new_block;
    if (list->tail)
        list->tail->next = new_block;
    list->tail = new_block;
    list->blockCount++;
    return 0;
}

int tac_init(TAC *tac) {
    tac->type = ADDR_NONE;
    if (!(tac->destination = malloc(sizeof(TAC_addr)))) {
        return 1;
    }
    if (!(tac->operand1 = malloc(sizeof(TAC_addr)))) {
        free(tac->destination);
        return 1;
    }
    if (!(tac->operand2 = malloc(sizeof(TAC_addr)))) {
        free(tac->destination);
        free(tac->operand1);
        return 1;
    }
    return 0;
}

void tac_free(TAC *tac) {
    tac_addr_free(tac->destination);
    free(tac->destination);
    tac_addr_free(tac->operand1);
    free(tac->operand1);
    tac_addr_free(tac->operand2);
    free(tac->operand2);
}

void tac_addr_free(TAC_addr *addr) {
    if (addr->type == ADDR_STRING || addr->type == ADDR_VAR) {
        strFree(addr->data.string);
        free(addr->data.string);
    }
}

TAC *tac_create(InstructionType type, TAC_addr dest, TAC_addr op1, TAC_addr op2) {
    TAC *tac = malloc(sizeof(TAC));
    if (!tac)
        return NULL;
    tac->type = type;
    tac->destination = malloc(sizeof(TAC_addr));
    if (!tac->destination) {
        free(tac);
        return NULL;
    }
    tac->destination->type = dest.type;
    tac->destination->data = dest.data;
    tac->operand1 = malloc(sizeof(TAC_addr));
    if (!tac->operand1) {
        free(tac->destination);
        free(tac);
        return NULL;
    }
    tac->operand1->type = op1.type;
    tac->operand1->data = op1.data;
    tac->operand2 = malloc(sizeof(TAC_addr));
    if (!tac->operand2) {
        free(tac->destination);
        free(tac->operand1);
        free(tac);
        return NULL;
    }
    tac->operand2->type = op2.type;
    tac->operand2->data = op2.data;
    return tac;
}

TAC *tac_get_line(TAC_list *list, const unsigned int idx) {
    static TAC_list_block_ptr cached_block;
    static unsigned int cached_block_id = -1;

    if (!list || !list->head || idx >= list->length)
        return NULL;

    unsigned int block_id = idx / TAC_COUNT_PER_ALLOC;
    if (block_id >= list->blockCount)
        return NULL;

    TAC_list_block_ptr block = list->head;
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

    return block->data[idx % TAC_COUNT_PER_ALLOC];
}

int tac_append_line(TAC_list *list, TAC *tac) {
    if (!list || !tac)
        return 1;

    TAC_list_block_ptr block = list->tail;

    unsigned int idx = list->length % TAC_COUNT_PER_ALLOC;
    if (idx == 0) { // We've entered a new block
        if (tac_list_append_new_block(list))
            return 1;

        block = list->tail;
    }
    list->length++;

    block->data[idx] = tac;
    return 0;
}
