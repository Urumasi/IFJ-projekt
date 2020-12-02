
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "symtable.h"
#include "error.h"

int symtableSIZE = MAX_SYMSIZE;

int hashCode(tKey key)
{
    int retval = 1;
    int keylen = strlen(key);
    for (int i = 0; i < keylen; i++)
        retval += key[i];
    return (retval % symtableSIZE);
}

void symtableInit(tSymtable *ptrht)
{
    for (int i = 0; i < symtableSIZE; i++)
    {
        (*ptrht)[i] = NULL;
    }
}

tSymtableItem *symtableSearch(tSymtable *ptrht, tKey key)
{
    tSymtableItem *item = (*ptrht)[hashCode(key)];
    while (item)
    {
        if (strcmp(item->key, key) == 0)
            return item;
        item = item->ptrnext;
    }
    return NULL;
}

tSymtableData symtableInsert(tSymtable *ptrht, tKey key)
{
    tSymtableItem *item = symtableSearch(ptrht, key);
    if (item)
    {
        return item->data;
    }
    else
    {
        item = (*ptrht)[hashCode(key)];
        tSymtableItem *newItem = malloc(sizeof(struct tSymtableItem));
        if (newItem)
        {
            newItem->key = malloc(strlen(key) + 1);
            if (newItem->key)
            {
                memcpy(newItem->key, key, strlen(key) + 1);
                newItem->ptrnext = item;
                newItem->data = malloc(sizeof(struct tSymtableData));
                if (newItem->data)
                {
                    newItem->data->defined = false;
                    newItem->data->typesSet = false;
                    strInit(&newItem->data->argumentTypes);
                    strInit(&newItem->data->returnTypes);
                    (*ptrht)[hashCode(key)] = newItem;
                    return newItem->data;
                }
            }
        }
        return NULL;
    }
}

tSymtableData symtableRead(tSymtable *ptrht, tKey key)
{
    tSymtableItem *item = symtableSearch(ptrht, key);
    if (item)
        return item->data;
    else
    {
        return NULL;
    }
}

tSymtableData symtableReadStack(tSymStack *ptrht, tKey key)
{
    tSymStackItem *stackItem = ptrht->top;
    tSymtableItem *symItem = NULL;
    while (!symItem && stackItem)
    {
        symItem = symtableSearch(&stackItem->symtable, key);
        stackItem = stackItem->next;
    }
    return symItem ? symItem->data : NULL;
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
            strFree(&prevItem->data->argumentTypes);
            strFree(&prevItem->data->returnTypes);
            free(prevItem->key);
            free(prevItem->data);
            free(prevItem);
        }
    }
    symtableInit(ptrht);
}

// SYMSTACK

void symStackInit(tSymStack *symStack)
{
    symStack->top = NULL;
    symStack->scopeCount = 0;
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
    tSymStackItem *item = malloc(sizeof(struct tSymStackItem));
    if (!item)
        return false;
    symtableInit(&item->symtable);
    item->next = stack->top;
    stack->top = item;
    stack->scopeCount++;
    tSymStackItem *i = stack->top;
    return true;
}

bool symStackPop(tSymStack *stack)
{
    tSymStackItem *item = NULL;
    if (!stack->top)
        return false;
    item = stack->top;
    stack->top = item->next;
    stack->scopeCount--;
    symtableClearAll(&item->symtable);
    free(item);

    return true;
}
