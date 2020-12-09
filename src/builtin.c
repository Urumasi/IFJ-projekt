//
// Created by petr on 09.12.2020.
//

#include "builtin.h"
#include "generator.h"

void builtinInit(Builtin_list *list) {
    list->head = NULL;
}

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

Builtin *getBuiltin(Builtin_list *list, string *name) {
    if (!list || !name)
        return NULL;
    for (Builtin *item = list->head; item; item = item->next)
        if (!strCmpString(name, item->malformed_name))
            return item;
    return NULL;
}
