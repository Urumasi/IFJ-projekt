/**
 * IFJ Project 2020: Team 008, variant II
 *
 * @file generator.c
 *
 * @brief Code generator implementation.
 *
 *
 * @author Martin Kneslík <xknesl02@stud.fit.vutbr.cz>
 * @author Adriana Jurkechová <xjurke02@stud.fit.vutbr.cz>
 * @author Karel Norek <xnorek01@stud.fit.vutbr.cz>
 * @author Petr Salaba <xsalab00@stud.fit.vutbr.cz>
 */

#include "generator.h"
#include "stack.h"
#include "builtin.h"

TAC_list list;
sstack symbol_stack;

int scope_counter = 0;
tStack scope_stack;

TAC_ll hook_list;
unsigned int for_depth = 0;

Builtin_list builtins;

#define ADD_BUILTIN(key) symReadGlobal(#key); \
if (addBuiltin(&builtins, #key, BUILTIN_ ## key, GENERATE_NAME("func", parser->currentFunc))){ \
    builtinFree(&builtins); \
    return 1; \
};

#define TACNONE (TAC_addr) {ADDR_NONE, 0}


// Symbol stack

void ss_init(sstack *stack) {
    stack->head = NULL;
    stack->tail = NULL;
    stack->length = 0;
}

void ss_free(sstack *stack) {
    if (!stack || !stack->head)
        return;
    sstack_item_ptr item = stack->head;
    sstack_item_ptr tmp;
    while (item) {
        tmp = item->next;
        free(item);
        item = tmp;
    }
    stack->length = 0;
}

int ss_push(sstack *stack, TAC_addr *data) {
    if (!stack || !data)
        return 1;
    sstack_item_ptr item = malloc(sizeof(struct s_sstack_item));
    if (!item)
        return 1;
    item->next = NULL;
    item->prev = stack->tail;
    item->data = data;
    if (!stack->head) {
        stack->head = item;
    }
    if (stack->tail)
        stack->tail->next = item;
    stack->tail = item;
    stack->length++;
    return 0;
}

int ss_push_copy(sstack *stack, TAC_addr data) {
    TAC_addr *addr = malloc(sizeof(TAC_addr));
    if (!addr)
        return 1;
    addr->type = data.type;
    if (HAS_STRING(addr->type)) {
        addr->data.string = malloc(sizeof(string));
        if (!addr->data.string) {
            free(addr);
            return 1;
        }
        if (strInitCopy(addr->data.string, data.data.string)) {
            free(addr->data.string);
            free(addr);
            return 1;
        }
    } else {
        addr->data = data.data;
    }
    return ss_push(stack, addr);
}

TAC_addr *ss_pop(sstack *stack) {
    if (!stack || ss_empty(stack))
        return NULL;
    sstack_item_ptr item = stack->tail;
    stack->tail = item->prev;
    if (item->prev)
        item->prev->next = NULL;
    stack->length--;
    if (ss_empty(stack))
        stack->head = NULL;
    TAC_addr *data = item->data;
    free(item);
    return data;
}

bool ss_empty(sstack *stack) {
    return !stack->length;
}

// TAC_list list

void ll_init(TAC_ll *list) {
    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
}

void ll_free(TAC_ll *list) {
    if (!list || !list->head)
        return;
    TAC_ll_item item = list->head;
    TAC_ll_item tmp;
    while (item) {
        tmp = item->next;
        tac_list_free(item->data);
        free(item);
        item = tmp;
    }
    list->length = 0;
}

int ll_append(TAC_ll *list, TAC_list *data) {
    if (!list || !data)
        return 1;
    TAC_ll_item item = malloc(sizeof(struct s_TAC_ll_item));
    if (!item)
        return 1;
    item->next = NULL;
    item->data = data;
    if (!list->head)
        list->head = item;
    if (list->tail)
        list->tail->next = item;
    list->tail = item;
    list->length++;
    return 0;
}

TAC_list *ll_get(TAC_ll *list, unsigned int id) {
    if (!list || id >= list->length)
        return NULL;
    TAC_ll_item item = list->head;
    for (unsigned int i = 0; i < id; ++i)
        item = item->next;
    return item->data;
}

TAC_list *ll_last(TAC_ll *list) {
    if (!list)
        return NULL;
    return list->tail->data;
}

// Base functions

void print_taclist() {
    printf("=== TACLIST | length: %u\nLine | Instruction | destination, op1, op2\n===\n", list.length);
    TAC *tac;
    for (unsigned int i = 0; i < list.length; ++i) {
        tac = tac_get_line(&list, i);
        if (!tac) {
            printf("%u | NULL\n", i);
            continue;
        }
        printf("%3u | %3d | ", i, tac->type);
        print_addr(tac->destination);
        printf(", ");
        print_addr(tac->operand1);
        printf(", ");
        print_addr(tac->operand2);
        printf("\n");
    }

    printf("=== GENERATOR STACK | length: %u\n", symbol_stack.length);
    for (sstack_item_ptr item = symbol_stack.head; item; item = item->next) {
        print_addr(item->data);
        printf("\n");
    }
}

void generate_free() {
    tac_list_free(&list);
    ss_free(&symbol_stack);
    ll_free(&hook_list);
}

string *create_safe_string(string *s) {
    if (!s)
        return NULL;
    string *out = malloc(sizeof(string));
    strInit(out);
    char buffer[8];
    char c;
    for (unsigned int i = 0; i < s->length; ++i) {
        c = s->str[i];
        if (c <= 32 || c == '#' || c == '\\') {
            snprintf(buffer, 8, "\\%3d", c);
            for (unsigned int j = 0; buffer[j]; ++j) {
                if (buffer[j] == ' ')
                    strAddChar(out, '0');
                else
                    strAddChar(out, buffer[j]);
            }
        } else {
            strAddChar(out, c);
        }
    }
    return out;
}

// parser generators

int generate_prog_init(Parser parser) {
    builtinInit(&builtins);

    ADD_BUILTIN(inputs);
    ADD_BUILTIN(inputi);
    ADD_BUILTIN(inputf);
    ADD_BUILTIN(int2float);
    ADD_BUILTIN(float2int);
    ADD_BUILTIN(len);
    ADD_BUILTIN(substr);
    ADD_BUILTIN(ord);
    ADD_BUILTIN(chr);

    stackInit(&scope_stack);
    ss_init(&symbol_stack);
    ll_init(&hook_list);
    if (tac_list_init(&list))
        return 1;
    stackPush(&scope_stack, scope_counter);
    scope_counter++;
    return 0;
}

int generate_prog_end(Parser parser) {
    string *name = GENERATE_NAME("func", parser->currentFunc);
    if (!name)
        return 1;
    if (tac_append_line(&list, tac_create(TAC_LABEL,
                                          (TAC_addr) {ADDR_RAWSTRING, .data.string=strCreate("_start")},
                                          TACNONE,
                                          TACNONE))) {
        free(name);
        return 1;
    }
    if (tac_append_line(&list, tac_create(TAC_CALL,
                                          (TAC_addr) {ADDR_RAWSTRING, .data.string=name},
                                          TACNONE,
                                          TACNONE))) {
        free(name);
        return 1;
    }
    return 0;
}

int generate_push(Parser parser) {
    string *name;
    switch (parser->token.type) {
        case TOKEN_INT:
            if (ss_push_copy(&symbol_stack, (TAC_addr) {ADDR_INT, .data.integer=parser->token.attribute.integer}))
                return 1;
            break;
        case TOKEN_FLOAT:
            if (ss_push_copy(&symbol_stack, (TAC_addr) {ADDR_FLOAT, .data.decimal=parser->token.attribute.decimal}))
                return 1;
            break;
        case TOKEN_STRING:
            if (ss_push_copy(&symbol_stack,
                             (TAC_addr) {ADDR_STRING, .data.string=create_safe_string(parser->token.attribute.string)}))
                return 1;
            break;
        case TOKEN_IDENTIFIER:
            if (!strCmpConstStr(parser->token.attribute.string, "_"))
                break; // Parser will throw a semantic error
            name = GENERATE_SCOPED_NAME("var", parser->currentID);
            if (!name)
                return 1;
            if (ss_push_copy(&symbol_stack, (TAC_addr) {ADDR_VAR, .data.string=name}))
                return 1;
            break;
        default:
            break;
    }
    return 0;
}

int generate_func(Parser parser) {
    string *name = GENERATE_NAME("func", parser->currentFunc);
    if (!name)
        return 1;

    TAC_list *tacl = malloc(sizeof(TAC_list));
    if (!tacl)
        return 1;
    if (tac_list_init(tacl))
        return 1;
    if (ll_append(&hook_list, tacl)) {
        tac_list_free(tacl);
        return 1;
    }

    if (tac_append_line(&list, tac_create(TAC_FUNC,
                                          (TAC_addr) {ADDR_RAWSTRING, .data.string=name},
                                          TACNONE,
                                          TACNONE)))
        return 1;

    if (list.length > 1) {
        unsigned int idx = list.length - 2;
        TAC *tac;
        TAC_addr addr;
        for (unsigned int i = 0; i < parser->currentFunc->argumentTypes.length; ++i) {
            tac = tac_get_line(&list, idx);
            if (!tac)
                return 1;
            addr.type = tac->destination->type;
            addr.data = tac->destination->data;
            if (HAS_STRING(addr.type))
                addr.data.string = strCreateCopy(tac->destination->data.string);
            if (tac_append_line(&list, tac_create(TAC_DEFINE,
                                                  addr,
                                                  (TAC_addr) {ADDR_STACK, 0},
                                                  TACNONE)))
                return 1;
            --idx;
        }
    }
    if (tac_append_line(&list, tac_create(TAC_HOOK,
                                          (TAC_addr) {ADDR_INT, .data.integer=hook_list.length - 1},
                                          TACNONE,
                                          TACNONE)))
        return 1;
    return 0;
}

int generate_func_param(Parser parser) {
    string *name = GENERATE_SCOPED_NAME("var", parser->currentID);
    if (!name)
        return 1;
    if (tac_append_line(&list, tac_create(TAC_FUNC_PARAM,
                                          (TAC_addr) {ADDR_VAR, .data.string=name},
                                          (TAC_addr) {ADDR_INT, .data.integer=parser->currentID->type},
                                          TACNONE)))
        return 1;
    return 0;
}

int generate_func_end(Parser parser) {
    if (parser->currentFunc->returnTypes.length) { // Should return something but reached end of func
        return tac_append_line(&list, tac_create(TAC_EXIT,
                                                 TACNONE,
                                                 (TAC_addr) {ADDR_INT, 1},
                                                 TACNONE));
    }
    return generate_return(parser);
}

int generate_return(Parser parser) {
    if (parser->currentFunc->returnTypes.length) {
        TAC_addr *addr;
        for (unsigned int i = 0; i < parser->currentFunc->returnTypes.length; ++i) {
            addr = ss_pop(&symbol_stack);
            TAC_addr ta;
            ta.type = addr->type;
            ta.data = addr->data;
            if (HAS_STRING(ta.type))
                ta.data.string = strCreateCopy(addr->data.string);
            if (tac_append_line(&list, tac_create(TAC_PUSH,
                                                  TACNONE,
                                                  ta,
                                                  TACNONE))) {
                free(addr);
                return 1;
            }
            free(addr);
        }
    }
    if (tac_append_line(&list, tac_create(TAC_RETURN,
                                          TACNONE,
                                          TACNONE,
                                          TACNONE)))
        return 1;
    return 0;
}

int generate_stack_push(Parser parser) {
    TAC_addr addr;
    string *name;
    switch (parser->token.type) {
        case TOKEN_INT:
            addr.type = ADDR_INT;
            addr.data.integer = parser->token.attribute.integer;
            break;
        case TOKEN_FLOAT:
            addr.type = ADDR_FLOAT;
            addr.data.decimal = parser->token.attribute.decimal;
            break;
        case TOKEN_STRING:
            addr.type = ADDR_STRING;
            addr.data.string = create_safe_string(parser->token.attribute.string);
            break;
        case TOKEN_IDENTIFIER:
            name = GENERATE_SCOPED_NAME("var", parser->currentID);
            if (!name)
                return 1;
            addr.type = ADDR_VAR;
            addr.data.string = name;
            break;
        default:
            return 1;
    }
    if (tac_append_line(&list, tac_create(TAC_PUSH,
                                          TACNONE,
                                          addr,
                                          TACNONE))) {
        return 1;
    }
    return 0;
}

int generate_void_func_call(Parser parser) {
    unsigned long long called_id = GENERATE_ID(parser->calledFunc);
    string *name = GENERATE_NAME("func", parser->calledFunc);
    if (!name)
        return 1;

    Builtin *builtin = getBuiltin(&builtins, name);
    if(builtin){
        builtin->used = true;
        if (tac_append_line(&list, tac_create(TAC_CALL,
                                              (TAC_addr) {ADDR_RAWSTRING, .data.string=strCreateCopy(builtin->name)},
                                              TACNONE,
                                              TACNONE))) {
            free(name);
            return 1;
        }
        free(name);
        return 0;
    }

    tSymtableData print = symtableRead(&parser->sGlobal, "print");
    if (!print)
        return 1;
    unsigned long long print_id = GENERATE_ID(print);
    if (called_id == print_id) {
        // Replace all pushes to stack with writes
        if (list.length) {
            unsigned int idx = list.length - 1;
            TAC *tac;
            do {
                tac = tac_get_line(&list, idx);
                if (tac->type == TAC_PUSH) {
                    tac->type = TAC_WRITE;
                } else {
                    break;
                }
                --idx;
            } while (idx);
        }
        free(name);
        return 0;
    }

    if (tac_append_line(&list, tac_create(TAC_CALL,
                                          (TAC_addr) {ADDR_RAWSTRING, .data.string=name},
                                          TACNONE,
                                          TACNONE))) {
        free(name);
        return 1;
    }

    return 0;
}

int generate_func_call(Parser parser) {
    if (generate_void_func_call(parser))
        1;
    for (unsigned int i = 0; i < parser->calledFunc->returnTypes.length; ++i) {
        TAC_addr *addr = malloc(sizeof(TAC_addr));
        if (!addr) {
            return 1;
        }
        addr->type = ADDR_STACK;
        if (ss_push(&symbol_stack, addr)) {
            free(addr);
            return 1;
        }
    }
    return 0;
}

int generate_define_var(Parser parser) {
    parser->currentID->scopeId = stackTop(&scope_stack)->data;
    string *name = GENERATE_SCOPED_NAME("var", parser->currentID);
    if (!name)
        return 1;
    if (for_depth) {
        TAC_list *hook = ll_last(&hook_list);
        if (tac_append_line(hook, tac_create(TAC_DEFINE,
                                             (TAC_addr) {ADDR_VAR, .data.string=strCreateCopy(name)},
                                             TACNONE,
                                             TACNONE))) {
            free(name);
            return 1;
        }
    }
    TAC_addr *op = ss_pop(&symbol_stack);
    if (!op)
        return 1;
    int ret = 0;
    if (op->type == ADDR_OTUVAR && list.length) {
        // Replace assign to one-time use var to our var
        unsigned int idx = list.length - 1;
        TAC *tac;
        do {
            tac = tac_get_line(&list, idx);
            if (!tac) {
                ret = 1;
                break;
            }
            if (tac->destination->type == ADDR_OTUVAR &&
                !strCmpString(tac->destination->data.string, op->data.string)) {
                tac->destination->type = op->type;
                strClear(tac->destination->data.string);
                if (strConcatString(tac->destination->data.string, name)) {
                    ret = 1;
                }
                break;
            }
            --idx;
        } while (idx);
    } else {
        if (tac_append_line(&list, tac_create(for_depth ? TAC_ASSIGN : TAC_DEFINE,
                                              (TAC_addr) {ADDR_VAR, .data.string=name},
                                              *op,
                                              TACNONE))) {
            free(name);
            ret = 1;
        }
    }
    free(op);
    return ret;
}

int generate_assign_push_id(Parser parser) {
    if (!strCmpConstStr(&parser->id, "_")) {
        return ss_push_copy(&symbol_stack, (TAC_addr) {ADDR_DISCARD, 0});
    }
    string *name = GENERATE_SCOPED_NAME("var", parser->currentID);
    if (!name)
        return 1;
    return ss_push_copy(&symbol_stack, (TAC_addr) {ADDR_VAR, .data.string=name});
}

int generate_assign(Parser parser) {
    unsigned int var_count = parser->typesLeft.length;

    sstack_item_ptr lhs = symbol_stack.tail;
    for (unsigned int i = 0; i < var_count - 1; ++i)
        lhs = lhs->prev;
    sstack_item_ptr rhs = lhs;
    for (unsigned int i = 0; i < var_count; ++i)
        lhs = lhs->prev;

    for (unsigned int i = 0; i < var_count; ++i) {
        if (lhs->data->type != ADDR_DISCARD && rhs->data->type == ADDR_OTUVAR) {
            // Replace assign to one-time use var to our var
            if (list.length) {
                unsigned int idx = list.length - 1;
                TAC *tac;
                do {
                    tac = tac_get_line(&list, idx); // NULL
                    if (tac->destination->type == ADDR_OTUVAR &&
                        !strCmpString(tac->destination->data.string, rhs->data->data.string)) {
                        tac->destination->type = lhs->data->type;
                        strClear(tac->destination->data.string);
                        strConcatString(tac->destination->data.string, lhs->data->data.string);
                        break;
                    }
                    --idx;
                } while (idx);
            }
        } else {
            if (tac_append_line(&list, tac_create(TAC_ASSIGN,
                                                  *lhs->data,
                                                  *rhs->data,
                                                  TACNONE)))
                return 1;
        }
        lhs = lhs->next;
        rhs = rhs->next;
    }

    for (unsigned int i = 0; i < var_count * 2; ++i)
        free(ss_pop(&symbol_stack));

    return 0;
}

int generate_if(Parser parser) {
    stackPush(&scope_stack, scope_counter);
    scope_counter++;
    return tac_append_line(&list, tac_create(TAC_IF,
                                             (TAC_addr) {ADDR_INT, -1},
                                             TACNONE,
                                             TACNONE));
}

int generate_else(Parser parser) {
    stackPop(&scope_stack);
    stackPush(&scope_stack, scope_counter);
    scope_counter++;
    return tac_append_line(&list, tac_create(TAC_ELSE,
                                             (TAC_addr) {ADDR_INT, -1},
                                             TACNONE,
                                             TACNONE));
}

int generate_endif(Parser parser) {
    stackPop(&scope_stack);
    static int if_counter = 0;

    if (list.length) {
        // Claim unclaimed if and else codes and give them your ID
        unsigned int idx = list.length - 1;
        TAC *tac;
        for (; idx; --idx) {
            tac = tac_get_line(&list, idx);
            if (!tac)
                return 1;
            if (tac->type == TAC_ELSE && tac->destination->data.integer == -1) {
                tac->destination->data.integer = if_counter;
                break;
            }
        }
        for (; idx; --idx) {
            tac = tac_get_line(&list, idx);
            if (!tac)
                return 1;
            if (tac->type == TAC_IF && tac->destination->data.integer == -1) {
                tac->destination->data.integer = if_counter;
                break;
            }
        }
    }

    if (tac_append_line(&list, tac_create(TAC_ENDIF,
                                          (TAC_addr) {ADDR_INT, if_counter},
                                          TACNONE,
                                          TACNONE)))
        return 1;
    if_counter++;
    return 0;
}

int generate_for_begin(Parser parser) {
    stackPush(&scope_stack, scope_counter);
    scope_counter++;
    for_depth++;
    return 0;
}

int generate_for_condition(Parser parser) {
    return tac_append_line(&list, tac_create(TAC_FOR_COND,
                                             (TAC_addr) {ADDR_INT, -1},
                                             TACNONE,
                                             TACNONE));
}

int generate_for_assign(Parser parser) {
    return tac_append_line(&list, tac_create(TAC_FOR_ASSIGN,
                                             (TAC_addr) {ADDR_INT, -1},
                                             TACNONE,
                                             TACNONE));
}

int generate_for(Parser parser) {
    stackPush(&scope_stack, scope_counter);
    scope_counter++;
    return tac_append_line(&list, tac_create(TAC_FOR,
                                             (TAC_addr) {ADDR_INT, -1},
                                             TACNONE,
                                             TACNONE));
}

int generate_endfor(Parser parser) {
    static int for_counter = 0;
    stackPop(&scope_stack);
    stackPop(&scope_stack);
    for_depth--;

    if (list.length) {
        // Claim unclaimed for_cond, for_assign and for codes and give them your ID
        unsigned int idx = list.length - 1;
        TAC *tac;
        for (; idx; --idx) {
            tac = tac_get_line(&list, idx);
            if (!tac)
                return 1;
            if (tac->type == TAC_FOR && tac->destination->data.integer == -1) {
                tac->destination->data.integer = for_counter;
                break;
            }
        }
        for (; idx; --idx) {
            tac = tac_get_line(&list, idx);
            if (!tac)
                return 1;
            if (tac->type == TAC_FOR_ASSIGN && tac->destination->data.integer == -1) {
                tac->destination->data.integer = for_counter;
                break;
            }
        }
        for (; idx; --idx) {
            tac = tac_get_line(&list, idx);
            if (!tac)
                return 1;
            if (tac->type == TAC_FOR_COND && tac->destination->data.integer == -1) {
                tac->destination->data.integer = for_counter;
                break;
            }
        }
    }

    if (tac_append_line(&list, tac_create(TAC_ENDFOR,
                                          (TAC_addr) {ADDR_INT, for_counter},
                                          TACNONE,
                                          TACNONE)))
        return 1;
    for_counter++;
    return 0;
}

// expression generators

int generate_operation(Prec_symbol symbol, DataType expr_type) {
    static unsigned int tmp_counter = 0;
    TAC_addr *op1 = NULL;
    TAC_addr *op2 = NULL;
    string *tmpvar;
    InstructionType type = TAC_NONE;
    switch (symbol) {
        case PLUS:
            if (expr_type == tSTRING) {
                type = TAC_CONCAT;
            } else {
                type = TAC_ADD;
            }
            break;
        case MINUS:
            type = TAC_SUB;
            break;
        case MULTIPLY:
            type = TAC_MUL;
            break;
        case DIVIDE:
            if (expr_type == tINT) {
                type = TAC_IDIV;
            } else {
                type = TAC_DIV;
            }
            break;
        case LESSER:
            type = TAC_LESSER;
            break;
        case LESS_OR_EQ:
            type = TAC_LESS_OR_EQ;
            break;
        case GREATER:
            type = TAC_GREATER;
            break;
        case GRT_OR_EQ:
            type = TAC_GRT_OR_EQ;
            break;
        case EQUAL:
            type = TAC_EQUAL;
            break;
        case NOT_EQUAL:
            type = TAC_NOT_EQUAL;
            break;
        default:
            break;
    }

    if (type != TAC_NONE) {
        op2 = ss_pop(&symbol_stack);
        if (!op2)
            return 1;
        op1 = ss_pop(&symbol_stack);
        if (!op1) {
            free(op2);
            return 1;
        }
        if (type >= TAC_ADD && type <= TAC_CONCAT) {
            tmpvar = strCreateFromFormat("tmpvarop$%u", tmp_counter);
            if (tac_append_line(&list, tac_create(type,
                                                  (TAC_addr) {ADDR_OTUVAR, .data.string=tmpvar},
                                                  *op1,
                                                  *op2))) {
                free(op1);
                free(op2);
                return 1;
            }

            if (ss_push_copy(&symbol_stack, (TAC_addr) {ADDR_OTUVAR, .data.string=tmpvar})) {
                free(op1);
                free(op2);
                return 1;
            }
            tmp_counter++;
        } else { // condition
            if (tac_append_line(&list, tac_create(type,
                                                  TACNONE,
                                                  *op1,
                                                  *op2))) {
                free(op1);
                free(op2);
                return 1;
            }
        }
    }

    if (op1)
        free(op1);
    if (op2)
        free(op2);
    return 0;
}

//

int print_addr(TAC_addr *addr) {
    switch (addr->type) {
        case ADDR_NONE:
            printf("nil@nil");
            break;
        case ADDR_INT:
            printf("int@%ld", addr->data.integer);
            break;
        case ADDR_FLOAT:
            printf("float@%a", addr->data.decimal);
            break;
        case ADDR_STRING:
            printf("string@%s", addr->data.string->str);
            break;
        case ADDR_RAWSTRING:
            printf("%s", addr->data.string->str);
            break;
        case ADDR_VAR:
        case ADDR_OTUVAR:
            printf("LF@%s", addr->data.string->str);
            break;
        default:
            return 1;
    }
    return 0;
}

#define GENERATE_OP_GEN(op) \
if(tac->destination->type == ADDR_OTUVAR){ \
    printf("DEFVAR "); \
    print_addr(tac->destination); \
    printf("\n"); \
} \
printf(#op " "); \
print_addr(tac->destination); \
printf(" "); \
print_addr(tac->operand1); \
printf(" "); \
print_addr(tac->operand2);

#define GENERATE_PUSH(op) \
if (op->type == ADDR_STACK) \
    continue; \
printf("PUSHS "); \
print_addr(op);

int generate() {
    printf(".IFJcode20\nDEFVAR GF@devnull\nJUMP _start\n");
    for (Builtin *builtin = builtins.head; builtin; builtin = builtin->next)
        if (builtin->used)
            puts(builtin->content->str);
    if (generate_from_list(&list))
        return 1;
    return 0;
}

int generate_from_list(TAC_list *tlist) {
    TAC *tac;
    for (unsigned int idx = 0; idx < tlist->length; ++idx) {
        tac = tac_get_line(tlist, idx);
        switch (tac->type) {
            case TAC_ADD:
            GENERATE_OP_GEN(ADD);
                break;
            case TAC_SUB:
            GENERATE_OP_GEN(SUB);
                break;
            case TAC_MUL:
            GENERATE_OP_GEN(MUL);
                break;
            case TAC_DIV:
            GENERATE_OP_GEN(DIV);
                break;
            case TAC_IDIV:
            GENERATE_OP_GEN(IDIV);
                break;
            case TAC_CONCAT:
            GENERATE_OP_GEN(CONCAT);
                break;
            case TAC_LESSER:
            GENERATE_PUSH(tac->operand1);
                printf("\n");
                GENERATE_PUSH(tac->operand2);
                printf("\nLTS");
                break;
            case TAC_LESS_OR_EQ:
            GENERATE_PUSH(tac->operand1);
                printf("\n");
                GENERATE_PUSH(tac->operand2);
                printf("\nGTS\nNOTS");
                break;
            case TAC_GREATER:
            GENERATE_PUSH(tac->operand1);
                printf("\n");
                GENERATE_PUSH(tac->operand2);
                printf("\nGTS");
                break;
            case TAC_GRT_OR_EQ:
            GENERATE_PUSH(tac->operand1);
                printf("\n");
                GENERATE_PUSH(tac->operand2);
                printf("\nLTS\nNOTS");
                break;
            case TAC_EQUAL:
            GENERATE_PUSH(tac->operand1);
                printf("\n");
                GENERATE_PUSH(tac->operand2);
                printf("\nEQS");
                break;
            case TAC_NOT_EQUAL:
            GENERATE_PUSH(tac->operand1);
                printf("\n");
                GENERATE_PUSH(tac->operand2);
                printf("\nEQS\nNOTS");
                break;
            case TAC_PUSH:
            GENERATE_PUSH(tac->operand1);
                break;
            case TAC_ASSIGN:
                if (tac->destination->type == ADDR_DISCARD) {
                    if (tac->operand1->type == ADDR_STACK) {
                        printf("POPS GF@devnull");
                        break;
                    }
                    continue;
                }
                if (tac->operand1->type != ADDR_STACK) {
                    printf("PUSHS ");
                    print_addr(tac->operand1);
                    printf("\n");
                }
                printf("POPS ");
                print_addr(tac->destination);
                break;
            case TAC_DEFINE:
                printf("DEFVAR ");
                print_addr(tac->destination);
                if (tac->operand1->type == ADDR_NONE)
                    break;
                if (tac->operand1->type != ADDR_STACK) {
                    printf("\nPUSHS ");
                    print_addr(tac->operand1);
                }
                printf("\nPOPS ");
                print_addr(tac->destination);
                break;
            case TAC_JUMP:
                printf("JUMP ");
                print_addr(tac->destination);
                break;
            case TAC_CALL:
                printf("CALL ");
                print_addr(tac->destination);
                break;
            case TAC_RETURN:
                printf("POPFRAME\nRETURN");
                break;
            case TAC_LABEL:
                printf("LABEL ");
                print_addr(tac->destination);
                break;
            case TAC_EXIT:
                printf("EXIT ");
                print_addr(tac->operand1);
                break;
            case TAC_FUNC:
                printf("LABEL ");
                print_addr(tac->destination);
                printf("\nCREATEFRAME\nPUSHFRAME");
                break;
            case TAC_WRITE:
                printf("WRITE ");
                print_addr(tac->operand1);
                break;
            case TAC_IF:
                printf("PUSHS bool@false\n"
                       "JUMPIFEQS else$%ld", tac->destination->data.integer);
                break;
            case TAC_ELSE:
                printf("JUMP endif$%ld\n"
                       "LABEL else$%ld", tac->destination->data.integer, tac->destination->data.integer);
                break;
            case TAC_ENDIF:
                printf("LABEL endif$%ld", tac->destination->data.integer);
                break;
            case TAC_FOR_COND:
                printf("LABEL forcond$%ld", tac->destination->data.integer);
                break;
            case TAC_FOR_ASSIGN:
                printf("PUSHS bool@true\n"
                       "JUMPIFEQS for$%ld\n"
                       "JUMP endfor$%ld\n"
                       "LABEL forassign$%ld", tac->destination->data.integer, tac->destination->data.integer,
                       tac->destination->data.integer);
                break;
            case TAC_FOR:
                printf("JUMP forcond$%ld\n"
                       "LABEL for$%ld", tac->destination->data.integer, tac->destination->data.integer);
                break;
            case TAC_ENDFOR:
                printf("JUMP forassign$%ld\n"
                       "LABEL endfor$%ld", tac->destination->data.integer, tac->destination->data.integer);
                break;
            case TAC_HOOK:
                if (generate_from_list(ll_get(&hook_list, tac->destination->data.integer)))
                    return 1;
                continue;
            default:
                continue;
        }
        printf("\n");
    }
    return 0;
}

#undef GENERATE_OP_GEN
#undef GENERATE_PUSH
