
#include <stdlib.h>
#include <stdio.h>
#include "symtable.h"

int symtableSIZE = MAX_SYMSIZE;

int hashCode(string *key)
{
    int retval = 1;
    int keylen = key->length;
    for (int i = 0; i < keylen; i++)
        retval += key->str[i];
    return (retval % symtableSIZE);
}

void symtableInit(tSymtable *ptrht)
{
    for (int i = 0; i < symtableSIZE; i++)
    {
        (*ptrht)[i] = NULL;
    }
}

tSymtableItem *symtableSearch(tSymtable *ptrht, string *key)
{
    tSymtableItem *item = (*ptrht)[hashCode(key)];
    while (item)
    {
        if (strcmp(item->key->str, key->str) == 0)
            return item;
        item = item->ptrnext;
    }
    return NULL;
}

void symtableInsert(tSymtable *ptrht, string *key, tSymtableData data)
{
    tSymtableItem *item = symtableSearch(ptrht, key);
    if (item)
    {
        item->data = data;
    }
    else
    {
        item = (*ptrht)[hashCode(key)];
        tSymtableItem *newItem = malloc(sizeof(struct tSymtableItem));
        if (newItem)
        {
            newItem->key = malloc(sizeof(string));
            if (newItem->key)
            {
                strInit(newItem->key);
                strCopyString(newItem->key, key);
                newItem->data = data;
                newItem->ptrnext = item;
                (*ptrht)[hashCode(key)] = newItem;
            }
        }
    }
}

tSymtableData symtableRead(tSymtable *ptrht, string *key)
{
    tSymtableItem *item = symtableSearch(ptrht, key);
    if (item)
        return item->data;
    else
    {
        tSymtableData data;
        data.defined = false;
        return data;
    }
}

void symtableDelete(tSymtable *ptrht, string *key)
{
    tSymtableItem *item = (*ptrht)[hashCode(key)];
    if (item)
    {
        if (strcmp(item->key->str, key->str) == 0)
        {
            (*ptrht)[hashCode(key)] = item->ptrnext;
            strFree(item->key);
            free(item->key);
            free(item);
        }
        tSymtableItem *prevItem = (*ptrht)[hashCode(key)];
        item = item->ptrnext;
        while (item)
        {
            if (strcmp(item->key->str, key->str) == 0)
            {
                prevItem->ptrnext = item->ptrnext;
                strFree(item->key);
                free(item->key);
                free(item);
                item->ptrnext = NULL;
                return;
            }
            item = item->ptrnext;
            prevItem = prevItem->ptrnext;
        }
    }
}

void symtableClearAll(tSymtable *ptrht)
{
    for (size_t i = 0; i < symtableSIZE; i++)
    {
        tSymtableItem *item = (*ptrht)[i];
        while (item)
        {
            tSymtableItem *prevItem = item;
            item = item->ptrnext;
            strFree(prevItem->key);
            free(prevItem->key);
            free(prevItem);
        }
    }
    symtableInit(ptrht);
}

// SYMSTACK

void symStackInit(tSymStack *symStack)
{
    symStack->top = NULL;
}

void symStackDispose(tSymStack *symStack)
{
    while (symStackPop(symStack))
    {
    }
}

bool symSymStackEmpty(tSymStack *symStack)
{
    return !symStack->top;
}

bool symStackPush(tSymStack *stack)
{
    tSymStackItem *item = malloc(sizeof(tSymStackItem));
    if (!item)
        return false;
    symtableInit(&item->symtable);
    item->next = stack->top;
    stack->top = item;
    return true;
}

bool symStackPop(tSymStack *stack)
{
    tSymStackItem *item = NULL;
    if (!stack->top)
        return false;
    item = stack->top;
    stack->top = item->next;
    symtableClearAll(&item->symtable);
    free(item);

    return true;
}
