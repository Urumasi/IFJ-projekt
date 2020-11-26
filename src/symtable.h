#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

#include <string.h>
#include <stdbool.h>
#include "str.h"

// SYMTABLE
#define MAX_SYMSIZE 101

typedef enum
{
	INT,
	FLOAT64,
	STRING,
	BOOL,
	UNDERSCORE
} DataType;

typedef struct tSymtableData
{
	//func
	bool defined;
	string *argumentTypes;
	string *returnTypes;

	//var
	DataType type;
} tSymtableData;

typedef struct tSymtableItem
{
	string *key;
	tSymtableData data;
	struct tSymtableItem *ptrnext;
} tSymtableItem;

typedef tSymtableItem *tSymtable[MAX_SYMSIZE];

extern int symtableSIZE;

int hashCode(string *key);
void symtableInit(tSymtable *ptrht);
tSymtableItem *symtableSearch(tSymtable *ptrht, string *key);
void symtableInsert(tSymtable *ptrht, string *key, tSymtableData data);
tSymtableData symtableRead(tSymtable *ptrht, string *key);
void symtableDelete(tSymtable *ptrht, string *key);
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
