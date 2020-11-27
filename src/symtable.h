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
	tNONE
} DataType;

typedef char *tKey;

typedef struct tSymtableData
{
	string *id;
	//func
	bool defined;
	string *argumentTypes;
	string *returnTypes;

	//var
	DataType type;
} tSymtableData;

typedef struct tSymtableItem
{
	tKey key;
	tSymtableData data;
	struct tSymtableItem *ptrnext;
} tSymtableItem;

typedef tSymtableItem *tSymtable[MAX_SYMSIZE];

extern int symtableSIZE;

int hashCode(tKey key);
void symtableInit(tSymtable *ptrht);
tSymtableItem *symtableSearch(tSymtable *ptrht, tKey key);
void symtableInsert(tSymtable *ptrht, tKey key, tSymtableData *data);
tSymtableData symtableRead(tSymtable *ptrht, tKey key);
void symtableDelete(tSymtable *ptrht, tKey key);
void symtableClearAll(tSymtable *ptrht);

//SYMSTACK

typedef struct tSymStackItem
{
	tSymtable symtable;
	struct tSymStackItem *next;
} tSymStackItem;

typedef struct
{
	tSymStackItem *top;
} tSymStack;

void symStackInit(tSymStack *symStack);
void symStackDispose(tSymStack *symStack);
unsigned symStackSize(tSymStack *symStack);
bool symStackEmpty(tSymStack *symStack);
bool symStackPush(tSymStack *symStack);
bool symStackPop(tSymStack *symStack);

#endif // __SYMTABLE_H__
