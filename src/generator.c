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

TAC_list list;
sstack symbol_stack;

#define PRINT_TAC_ADDR(x) \
    switch(x->type){ \
        case ADDR_NONE: \
            printf("(none)"); \
            break; \
        case ADDR_INT: \
            printf("i$%ld", x->data.integer); \
            break; \
        case ADDR_FLOAT: \
            printf("f$%.4g", x->data.decimal); \
            break; \
        case ADDR_STRING: \
            printf("s$%s", x->data.string->str); \
            break; \
        case ADDR_VAR: \
        case ADDR_OTUVAR: \
            printf("v$%s", x->data.string->str); \
            break; \
        default: \
            printf("Unknown %d", x->type); \
    };

// Symbol stack

int ss_init(sstack *stack) {
    stack->head = NULL;
    stack->tail = NULL;
    stack->length = 0;
    return 0;
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
    if (addr->type == ADDR_STRING || addr->type == ADDR_VAR || addr->type == ADDR_OTUVAR) {
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
        printf("%u | %d | ", i, tac->type);
        PRINT_TAC_ADDR(tac->destination);
        printf(", ");
        PRINT_TAC_ADDR(tac->operand1);
        printf(", ");
        PRINT_TAC_ADDR(tac->operand2);
        printf("\n");
    }

    printf("=== GENERATOR STACK | length: %u\n", symbol_stack.length);
    for (sstack_item_ptr item = symbol_stack.head; item; item = item->next) {
        PRINT_TAC_ADDR(item->data);
        printf("\n");
    }
}

void generate_end() {
    tac_list_free(&list);
    ss_free(&symbol_stack);
}

// parser generators

int generate_prog_init(Parser parser) {
    printf(".IFJCode20\n"
           "CALL main\n"
           "EXIT int@0\n");
    if (ss_init(&symbol_stack))
        return 1;
    if (tac_list_init(&list))
        return 1;
    if (tac_append_line(&list, tac_create(TAC_CALL,
                                          (TAC_addr) {ADDR_STRING, .data.string=strCreate("main")},
                                          (TAC_addr) {ADDR_NONE, 0},
                                          (TAC_addr) {ADDR_NONE, 0}))) {
        tac_list_free(&list);
        return 1;
    }
    if (tac_append_line(&list, tac_create(TAC_EXIT,
                                          (TAC_addr) {ADDR_NONE, 0},
                                          (TAC_addr) {ADDR_INT, 0},
                                          (TAC_addr) {ADDR_NONE, 0}))) {
        tac_list_free(&list);
        return 1;
    }
    return 0;
}

int generate_push(Parser parser) {
    switch (parser->token.type) {
        case TOKEN_INT:
            printf("PUSHS int@%ld\n", parser->token.attribute.integer);
            if (ss_push_copy(&symbol_stack, (TAC_addr) {ADDR_INT, .data.integer=parser->token.attribute.integer}))
                return 1;
            break;
        case TOKEN_FLOAT:
            printf("PUSHS float@%a\n", parser->token.attribute.decimal);
            if (ss_push_copy(&symbol_stack, (TAC_addr) {ADDR_FLOAT, .data.decimal=parser->token.attribute.decimal}))
                return 1;
            break;
        case TOKEN_STRING:
            printf("PUSHS string@%s\n", parser->token.attribute.string->str);
            if (ss_push_copy(&symbol_stack, (TAC_addr) {ADDR_STRING, .data.string=parser->token.attribute.string}))
                return 1;
            break;
        case TOKEN_IDENTIFIER:
            printf("PUSHS LF@%s\n", parser->token.attribute.string->str);
            if (ss_push_copy(&symbol_stack, (TAC_addr) {ADDR_VAR, .data.string=parser->token.attribute.string}))
                return 1;
            break;
        default:
            break;
    }
    return 0;
}

int generate_func(Parser parser) {
    printf("LABEL %s\n"
           "CREATEFRAME\n"
           "PUSHFRAME\n", parser->token.attribute.string->str);
    return tac_append_line(&list, tac_create(TAC_FUNC,
                                             (TAC_addr) {ADDR_STRING, .data.string=strCreateCopy(
                                                 parser->token.attribute.string)},
                                             (TAC_addr) {},
                                             (TAC_addr) {}));
}

int generate_func_end(Parser parser) {
    printf("POPFRAME\n"
           "RETURN # endfunc\n");
    return 0;
}

int generate_return(Parser parser) {
    printf("POPFRAME\n"
           "RETURN\n");
    return 0;
}

int generate_func_call(Parser parser) {
    printf("CALL %s\n", parser->token.attribute.string->str);
    return tac_append_line(&list, tac_create(TAC_CALL,
                                             (TAC_addr) {ADDR_STRING, .data.string=strCreateCopy(parser->token.attribute.string)},
                                             (TAC_addr) {ADDR_NONE, 0},
                                             (TAC_addr) {ADDR_NONE, 0}));
}

int generate_define_var(Parser parser) {
    printf("DEFVAR LF@%s\n"
           "POPS LF@%s\n", parser->id.str, parser->id.str);
    TAC_addr *op = ss_pop(&symbol_stack);
    if (!op)
        return 1;
    int ret = tac_append_line(&list, tac_create(TAC_DEFINE,
                                                (TAC_addr) {ADDR_VAR, .data.string=strCreateCopy(&parser->id)},
                                                *op,
                                                (TAC_addr) {ADDR_NONE, 0}));
    free(op);
    return ret;
}

int generate_assign_push_id(Parser parser) {
    return ss_push_copy(&symbol_stack, (TAC_addr) {ADDR_VAR, .data.string=parser->token.attribute.string});
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
        if (tac_append_line(&list, tac_create(TAC_ASSIGN,
                                              *lhs->data,
                                              *rhs->data,
                                              (TAC_addr) {ADDR_NONE, 0})))
            return 1;
        lhs = lhs->next;
        rhs = rhs->next;
    }

    for (unsigned int i = 0; i < var_count * 2; ++i)
        ss_pop(&symbol_stack);

    return 0;
}

// expression generators

int generate_operation(Prec_symbol symbol) {
    static unsigned int tmp_counter = 0;
    TAC_addr *op1 = NULL;
    TAC_addr *op2 = NULL;
    string *tmpvar;
    InstructionType type = TAC_NONE;
    switch (symbol) {
        case PLUS:
            printf("ADDS\n");
            type = TAC_ADD;
            break;
        case MINUS:
            printf("SUBS\n");
            type = TAC_SUB;
            break;
        case MULTIPLY:
            printf("MULS\n");
            type = TAC_MUL;
            break;
        case DIVIDE:
            printf("DIVS\n");
            type = TAC_DIV;
            break;
        default:
            printf("* OPERATION %d\n", symbol);
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
        tmpvar = strCreateFromFormat("$tmpvarop%u", tmp_counter);
        tac_append_line(&list, tac_create(type,
                                          (TAC_addr) {ADDR_OTUVAR, .data.string=tmpvar},
                                          *op1,
                                          *op2));

        if (ss_push_copy(&symbol_stack, (TAC_addr) {ADDR_OTUVAR, .data.string=tmpvar})) {
            free(op1);
            free(op2);
            return 1;
        }
        tmp_counter++;
    }

    if (op1)
        free(op1);
    if (op2)
        free(op2);
    return 0;
}
