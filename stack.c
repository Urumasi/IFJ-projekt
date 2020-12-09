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
#include "expression.h"

/**
 * @brief Inicialization of stack
 * @param stack Stack to be inicialized
 */
void stackInit(tStack* stack){
	stack->top = NULL;
}

/**
 * @brief Dispose whole stack
 * @param stack Stack to be disposed
 */
void stackDispose(tStack* stack){
	while (!stackPop(stack));
}

/**
 * @brief Return top item from stack
 * @param stack Stack
 * @return Item on top of the stack
 */
tItemPtr* stackTop(tStack* stack){
	tItemPtr *tmp = stack->top;
	return tmp;
}

/**
 * @brief Push value into the stack
 * @param stack Stack
 * @param Value to be pushed
 * @return Function returns 1 if allocation failed otherwise returns 0
 */
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

/**
 * @brief Insert value after first term, skipping HANDLE.
 * @param stack Stack
 * @param value Value to be inserted
 * @return Function returns 1 if allocation failed otherwise returns 0
 */
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

/**
 * @brief Pop first item in stack
 * @param stack Stack
 * @return Function returns 1 if stack is empty failed otherwise returns 0
 */
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