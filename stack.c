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

#define HANDLE 17

void stackInit(tStack* stack){
	stack->top = NULL;
}

void stackDispose(tStack* stack){
	while (!stackPop(stack));
}

tItemPtr* stackTop(tStack* stack){
	tItemPtr *tmp = stack->top;
	return tmp;
}

bool stackEmpty(tStack* stack){
	return !stack->top;
}

int stackPush(tStack* stack, int value){
	tItemPtr *item = malloc(sizeof(tItemPtr));
	
	if (item == NULL) {
		return 1;
	}

	item->data = value;
	item->next = stack->top;
	stack->top = item;
	return 0;
}

int stackInsertAfterTerm(tStack* stack, int value){
	tItemPtr *tmp = stack->top;
	tItemPtr *prev = NULL;

	while(tmp != NULL) {
		if (tmp->data < HANDLE) {
			tItemPtr *new = malloc(sizeof(tItemPtr));
			if (new == NULL) {
				return 1;
			}

			new->data = value;

			if (prev != NULL) {
				new->next = prev->next;
				prev->next = new;
			}else {
				new->next = stack->top;
				stack->top= new;
			}
			return 0;
		}
		prev = tmp;
		tmp = tmp->next;
	}
	return 0;
}

int stackPop(tStack* stack){
	tItemPtr *item = NULL;

	if (stack->top == NULL) {
		return 1;
	}

	item = stack->top;
	stack->top = item->next;
	
	free(item);

	return 0;
}