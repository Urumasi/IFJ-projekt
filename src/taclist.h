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

#include <stdint.h>
#include "str.h"

#define TAC_COUNT_PER_ALLOC 64

#define INIT_IF_NOT_NULL(dest, src) \
    if (src) \
    strInitCopy(dest, src); \
    else \
    strInit(dest);

typedef enum {
    TAC_NONE,
    TAC_ADD,
    TAC_SUB,
    TAC_MUL,
    TAC_DIV,
    TAC_ASSIGN_ID,
    TAC_ASSIGN,
    TAC_DEFINE,
    TAC_JUMP,
    TAC_CALL,
    TAC_RETURN,
    TAC_LABEL,
    TAC_EXIT,
    TAC_FUNC,
} InstructionType;

typedef enum {
    ADDR_NONE,
    ADDR_INT,
    ADDR_FLOAT,
    ADDR_STRING,
    ADDR_VAR,
    ADDR_OTUVAR, // one-time use variable
} AddressType;

/**
 * @union Three address code address data
 */
typedef union {
    int64_t integer;
    double decimal;
    string *string;
} TAC_addr_data;

typedef struct s_TAC_addr {
    AddressType type;
    TAC_addr_data data;
} TAC_addr;

typedef struct s_TAC { // Three address code
    InstructionType type;
    TAC_addr *destination;
    TAC_addr *operand1;
    TAC_addr *operand2;
} TAC;

typedef struct s_TAC_list_block {
    TAC *data[TAC_COUNT_PER_ALLOC];
    struct s_TAC_list_block *next;
} *TAC_list_block_ptr;

typedef struct s_TAC_list {
    TAC_list_block_ptr head;
    TAC_list_block_ptr tail; // For appending quickly
    unsigned int length;
    unsigned int blockCount;
} TAC_list;

int tac_list_init(TAC_list *list);
void tac_list_free(TAC_list *list);

int tac_list_append_new_block(TAC_list *list);

int tac_init(TAC *tac);
void tac_free(TAC *tac);

void tac_addr_free(TAC_addr *addr);

TAC *tac_create(InstructionType type, TAC_addr dest, TAC_addr op1, TAC_addr op2);
TAC *tac_get_line(TAC_list *list, const unsigned int idx);
int tac_append_line(TAC_list *list, TAC *tac);

#endif //__TACLIST_H__
