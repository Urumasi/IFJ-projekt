/**
 * IFJ Project 2020: Team 008, variant II
 * 
 * @file symtable.c
 * 
 * @brief Implementation of hash table used for semantic analysis
 *
 * 
 * @author Martin Kneslík <xknesl02@stud.fit.vutbr.cz>
 * @author Adriana Jurkechová <xjurke02@stud.fit.vutbr.cz>
 * @author Karel Norek <xnorek01@stud.fit.vutbr.cz>
 * @author Petr Salaba <xsalab00@stud.fit.vutbr.cz>
 */

// Tabulka symbolů je převzatá z 2. domácího úkolu v předmětu IAL a modifikována pro použítí v tomto projektu
// Autor: Martin Kneslík (xknesl02)

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "symtable.h"

int symtableSIZE = MAX_SYMSIZE;

/**
 * @brief Hash function
 * @param key Identificator
 * @return Index in symtable
 */
int hashCode(tKey key)
{
    int retval = 1;
    int keylen = strlen(key);
    for (int i = 0; i < keylen; i++)
        retval += key[i];
    return (retval % symtableSIZE);
}

/**
 * @brief Inicialization of symtable
 * @param ptrht Symtable to be inicialized
 */
void symtableInit(tSymtable *ptrht)
{
    for (int i = 0; i < symtableSIZE; i++)
    {
        (*ptrht)[i] = NULL;
    }
}

/**
 * @brief Search symtable for key
 * @param ptrht Symtable to be searched
 * @param key Key to search for
 * @return Function returns item if it's found else returns NULL
 */
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

/**
 * @brief Insert key into the symtable
 * @param ptrht Symtable
 * @param key Key to insert
 * @return Return item->data if key already exists in symtable, newItem->data if we inserted new key
 *          or NULL if allocation failed
 */
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

/**
 * @brief Lookup key in symtable
 * @param key Key to look for
 * @param ptrht Symtable
 * @return Data of key in symtable or NULL if key wasn't found
 */
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

/**
 * @brief Lookup key in symstack
 * @param key Key to look for
 * @param ptrht Symtable
 * @return Data of key in symstack or NULL if key wasn't found
 */
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

/**
 * @brief Clear the while symtable
 * @param ptrht Symtable
 */
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

/**
 * @brief Inicialization of stack
 * @param symstack Stack to be inicialized
 */
void symStackInit(tSymStack *symStack)
{
    symStack->top = NULL;
    symStack->scopeCount = 0;
}

/**
 * @brief Dispose whole stack
 * @param symstack Stack to be disposed
 */
void symStackDispose(tSymStack *symStack)
{
    while (symStackPop(symStack)){}
}

/**
 * @brief Check if stack is empty
 * @param symstack Stack
 * @return true if stack is empty, false if not
 */
bool symSymStackEmpty(tSymStack *symStack)
{
    return !symStack->top;
}

/**
 * @brief Push value into the stack
 * @param stack Stack
 * @return Function returns false if allocation failed otherwise returns true
 */
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

/**
 * @brief Pop first item in stack
 * @param stack Stack
 * @return Function returns false if stack is empty failed otherwise returns true
 */
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
