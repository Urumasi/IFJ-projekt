/**
 * IFJ Project 2020: Team 008, variant II
 * 
 * @file symtable.h
 * 
 * @brief Header file for symtable.c
 *
 * 
 * @author Martin Kneslík <xknesl02@stud.fit.vutbr.cz>
 * @author Adriana Jurkechová <xjurke02@stud.fit.vutbr.cz>
 * @author Karel Norek <xnorek01@stud.fit.vutbr.cz>
 * @author Petr Salaba <xsalab00@stud.fit.vutbr.cz>
 */

#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

#include <string.h>
#include <stdbool.h>
#include "str.h"
#include "error.h"

// SYMTABLE
#define MAX_SYMSIZE 101

typedef enum
{
	tINT,
	tFLOAT64,
	tSTRING,
	tNONE,
	tMULTIPLE
} DataType;

typedef char *tKey;

typedef struct tSymtableData
{
	//func
	bool defined;
	bool typesSet;
	string argumentTypes;
	string returnTypes;

	//var
	DataType type;

	//generation
	int scopeId;
} * tSymtableData;

typedef struct tSymtableItem
{
	tKey key;
	tSymtableData data;
	struct tSymtableItem *ptrnext;
} tSymtableItem;

typedef tSymtableItem *tSymtable[MAX_SYMSIZE];

extern int symtableSIZE;

typedef struct tSymStackItem
{
	tSymtable symtable;
	struct tSymStackItem *next;
} tSymStackItem;

typedef struct
{
	tSymStackItem *top;
	int scopeCount;
} tSymStack;

int hashCode(tKey key);
void symtableInit(tSymtable *ptrht);
tSymtableItem *symtableSearch(tSymtable *ptrht, tKey key);
tSymtableData symtableInsert(tSymtable *ptrht, tKey key);
tSymtableData symtableRead(tSymtable *ptrht, tKey key);
tSymtableData symtableReadStack(tSymStack *ptrht, tKey key);
void symtableDelete(tSymtable *ptrht, tKey key);
void symtableClearAll(tSymtable *ptrht);

//SYMSTACK
void symStackInit(tSymStack *symStack);
void symStackDispose(tSymStack *symStack);
unsigned symStackSize(tSymStack *symStack);
bool symStackEmpty(tSymStack *symStack);
bool symStackPush(tSymStack *symStack);
bool symStackPop(tSymStack *symStack);

#endif // __SYMTABLE_H__
