/**
 * IFJ Project 2020: Team 008, variant II
 * 
 * @file stack.h
 * 
 * @brief Header file for stack implementation
 *
 * 
 * @author Martin Kneslík <xknesl02@stud.fit.vutbr.cz>
 * @author Adriana Jurkechová <xjurke02@stud.fit.vutbr.cz>
 * @author Karel Norek <xnorek01@stud.fit.vutbr.cz>
 * @author Petr Salaba <xsalab00@stud.fit.vutbr.cz>
 */

#ifndef __STACK_H__
#define __STACK_H__

#include <stdbool.h>

typedef struct tItem {
	struct tItem *next;
	int data;
} tItemPtr;

typedef struct {
	tItemPtr *top;
	unsigned size;
} tStack;

void stackInit(tStack* stack);
void stackDispose(tStack* stack);
unsigned stackSize(tStack* stack);
bool stackEmpty(tStack* stack);
bool stackPush(tStack* stack, int value);
bool stackPop(tStack* stack, int *value);

#endif //__STACK_H__
