#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

#include <string.h>
#include <stdbool.h>
#include "str.h"

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
