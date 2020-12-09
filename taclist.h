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

#define HAS_STRING(x) (x == ADDR_STRING || x == ADDR_RAWSTRING || x == ADDR_VAR || x == ADDR_OTUVAR)

typedef enum {
    TAC_NONE,       // 0
    TAC_ADD,
    TAC_SUB,
    TAC_MUL,
    TAC_DIV,
    TAC_IDIV,
    TAC_CONCAT,
    TAC_LESSER,
    TAC_LESS_OR_EQ,
    TAC_GREATER,
    TAC_GRT_OR_EQ,  // 10
    TAC_EQUAL,
    TAC_NOT_EQUAL,
    TAC_PUSH,
    TAC_ASSIGN,
    TAC_DEFINE,
    TAC_JUMP,
    TAC_CALL,
    TAC_RETURN,
    TAC_LABEL,
    TAC_EXIT,       // 20
    TAC_FUNC,
    TAC_FUNC_PARAM,
    TAC_WRITE,
    TAC_IF,
    TAC_ELSE,
    TAC_ENDIF,
    TAC_FOR,
    TAC_FOR_COND,
    TAC_FOR_ASSIGN,
    TAC_ENDFOR,     // 30
    TAC_HOOK,
} InstructionType;

typedef enum {
    ADDR_NONE,
    ADDR_INT,
    ADDR_FLOAT,
    ADDR_STRING,
    ADDR_RAWSTRING,
    ADDR_VAR,
    ADDR_OTUVAR, // one-time use variable
    ADDR_STACK,
    ADDR_DISCARD, // special var _
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

    TAC_list_block_ptr cached_block;
    unsigned int cached_block_id;
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
