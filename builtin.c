/**
 * IFJ Project 2020: Team 008, variant II
 *
 * @file builtin.c
 *
 * @brief Built-in structure method definitions.
 *
 *
 * @author Martin Kneslík <xknesl02@stud.fit.vutbr.cz>
 * @author Adriana Jurkechová <xjurke02@stud.fit.vutbr.cz>
 * @author Karel Norek <xnorek01@stud.fit.vutbr.cz>
 * @author Petr Salaba <xsalab00@stud.fit.vutbr.cz>
 */

#include "builtin.h"
#include "generator.h"

/**
 * @brief
 *
 * @param list
 */
void builtinInit(Builtin_list *list) {
    list->head = NULL;
}

/**
 * @brief
 *
 * @param list
 */
void builtinFree(Builtin_list *list) {
    if (!list || !list->head)
        return;
    Builtin *item = list->head;
    Builtin *tmp;
    while (item) {
        tmp = item->next;
        strFree(item->name);
        free(item->name);
        strFree(item->content);
        free(item->content);
        free(item);
        item = tmp;
    }
}

/**
 * @brief
 *
 * @param list
 * @param name
 * @param content
 * @param malformed_name
 * @return
 */
int addBuiltin(Builtin_list *list, const char *name, const char *content, string *malformed_name) {
    if (!list || !name || !content)
        return 1;
    Builtin *item = malloc(sizeof(Builtin));
    item->used = false;
    item->next = list->head;
    item->malformed_name = malformed_name;
    if (!(item->name = strCreate(name))) {
        strFree(item->malformed_name);
        free(item->malformed_name);
        free(item);
        return 1;
    }
    if (!(item->content = strCreate(content))) {
        strFree(item->malformed_name);
        free(item->malformed_name);
        strFree(item->name);
        free(item->name);
        free(item);
        return 1;
    }
    list->head = item;
    return 0;
}

/**
 * @brief
 *
 * @param list
 * @param name
 * @return
 */
Builtin *getBuiltin(Builtin_list *list, string *name) {
    if (!list || !name)
        return NULL;
    for (Builtin *item = list->head; item; item = item->next)
        if (!strCmpString(name, item->malformed_name))
            return item;
    return NULL;
}
