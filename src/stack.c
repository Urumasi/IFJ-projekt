/**
* IFJ Project 2020: Team 008, variant II
* 
* @file stack.c
* 
* @brief Stack implementation
*
* 
* @author Martin Kneslík <xknesl02@stud.fit.vutbr.cz>
* @author Adriana Jurkechová <xjurke02@stud.fit.vutbr.cz>
* @author Karel Norek <xnorek01@stud.fit.vutbr.cz>
* @author Petr Salaba <xsalab00@stud.fit.vutbr.cz>
*/

#include <stdlib.h>
#include "stack.h"

void stackInit(tStack* stack){
	stack->top = NULL;
	stack->size = 0;
}

void stackDispose(tStack* stack){
	while (stackPop(stack, NULL));
}

unsigned stackSize(tStack* stack){
	return stack->size;
}

bool stackEmpty(tStack* stack){
	return !stack->top;
}

bool stackPush(tStack* stack, int value){
	tItemPtr *item = malloc(sizeof(tItemPtr));
	
	if (item == NULL) {
		return false;
	}

	item->data = value;
	item->next = stack->top;
	stack->top = item;
	stack->size++;
	
	return true;
}

bool stackPop(tStack* stack, int *value){
	tItemPtr *item = NULL;

	if (stack->top == NULL) {
		return false;
	}

	item = stack->top;
	stack->top = item->next;
	*value = item->data;
	free(item);
	stack->size--;

	return true;
}


