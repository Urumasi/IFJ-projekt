//
// Created by petr on 09.12.2020.
//

#ifndef IFJ_PROJEKT_BUILTIN_H
#define IFJ_PROJEKT_BUILTIN_H

#include <stdbool.h>
#include "str.h"

typedef struct s_Builtin {
    string *name;
    string *malformed_name;
    string *content;
    struct s_Builtin *next;
    bool used;
} Builtin;

typedef struct s_Builtin_list {
    Builtin *head;
} Builtin_list;

void builtinInit(Builtin_list *list);

void builtinFree(Builtin_list *list);

int addBuiltin(Builtin_list *list, const char *name, const char *content, string *maformed_name);

Builtin *getBuiltin(Builtin_list *list, string *name);

// () (string/int/float, int)
#define BUILTIN_inputs "LABEL inputs\n"\
"CREATEFRAME\n"\
"PUSHFRAME\n"\
"DEFVAR LF@s\n"\
"READ LF@s string\n"\
"JUMPIFEQ inputs$error LF@s nil@nil\n"\
"PUSHS int@0\n"\
"PUSHS LF@s\n"\
"POPFRAME\n"\
"RETURN\n"\
"LABEL inputs$error\n"\
"PUSHS int@1\n"\
"PUSHS string@\n"\
"POPFRAME\n"\
"RETURN"

#define BUILTIN_inputi "LABEL inputi\n"\
"CREATEFRAME\n"\
"PUSHFRAME\n"\
"DEFVAR LF@s\n"\
"READ LF@s int\n"\
"JUMPIFEQ inputi$error LF@s nil@nil\n"\
"PUSHS int@0\n"\
"PUSHS LF@s\n"\
"POPFRAME\n"\
"RETURN\n"\
"LABEL inputi$error\n"\
"PUSHS int@1\n"\
"PUSHS int@0\n"\
"POPFRAME\n"\
"RETURN"

#define BUILTIN_inputf "LABEL inputf\n"\
"CREATEFRAME\n"\
"PUSHFRAME\n"\
"DEFVAR LF@s\n"\
"READ LF@s float\n"\
"JUMPIFEQ inputf$error LF@s nil@nil\n"\
"PUSHS int@0\n"\
"PUSHS LF@s\n"\
"POPFRAME\n"\
"RETURN\n"\
"LABEL inputf$error\n"\
"PUSHS int@1\n"\
"PUSHS float@0x0p+0\n"\
"POPFRAME\n"\
"RETURN"

#define BUILTIN_int2float "LABEL int2float\n"\
"INT2FLOATS\n"\
"RETURN"

#define BUILTIN_float2int "LABEL float2int\n"\
"FLOAT2INTS\n"\
"RETURN"

#define BUILTIN_len "LABEL len\n"\
"CREATEFRAME\n"\
"PUSHFRAME\n"\
"DEFVAR LF@tmp\n"\
"POPS LF@tmp\n"\
"STRLEN LF@tmp LF@tmp\n"\
"PUSHS LF@tmp\n"\
"POPFRAME\n"\
"RETURN"

// (s string, i int, n int) (string, int)
#define BUILTIN_substr "LABEL substr\n"\
"CREATEFRAME\n"\
"PUSHFRAME\n"\
"DEFVAR LF@n\n"\
"DEFVAR LF@i\n"\
"DEFVAR LF@s\n"\
"POPS LF@n\n"\
"POPS LF@i\n"\
"POPS LF@s\n"\
"DEFVAR LF@len\n"\
"STRLEN LF@len LF@s\n"\
"DEFVAR LF@cond\n"\
"LT LF@cond LF@i int@0\n"\
"JUMPIFEQ substr$error LF@cond bool@true\n"\
"LT LF@cond LF@i LF@len\n"\
"JUMPIFEQ substr$error LF@cond bool@false\n"\
"LT LF@cond LF@n int@0\n"\
"JUMPIFEQ substr$error LF@cond bool@true\n"\
"DEFVAR LF@result\n"\
"MOVE LF@result string@\n"\
"JUMPIFEQ substr$endloop LF@n int@0\n"\
"JUMPIFEQ substr$endloop LF@len int@0\n"\
"DEFVAR LF@char\n"\
"LABEL substr$loop\n"\
"GETCHAR LF@char LF@s LF@i\n"\
"CONCAT LF@result LF@result LF@char\n"\
"ADD LF@i LF@i int@1\n"\
"SUB LF@n LF@n int@1\n"\
"LT LF@cond LF@i LF@len\n"\
"JUMPIFEQ substr$endloop LF@cond bool@false\n"\
"GT LF@cond LF@n int@0\n"\
"JUMPIFEQ substr$endloop LF@cond bool@false\n"\
"JUMP substr$loop\n"\
"LABEL substr$endloop\n"\
"PUSHS int@0\n"\
"PUSHS LF@result\n"\
"POPFRAME\n"\
"RETURN\n"\
"LABEL substr$error\n"\
"PUSHS int@1\n"\
"PUSHS string@\n"\
"POPFRAME\n"\
"RETURN"

#define BUILTIN_ord "LABEL ord\n"\
"CREATEFRAME\n"\
"PUSHFRAME\n"\
"DEFVAR LF@i\n"\
"DEFVAR LF@s\n"\
"POPS LF@i\n"\
"POPS LF@s\n"\
"DEFVAR LF@len\n"\
"STRLEN LF@len LF@s\n"\
"DEFVAR LF@cond\n"\
"LT LF@cond LF@i int@0\n"\
"JUMPIFEQ ord$error LF@cond bool@true\n"\
"LT LF@cond LF@i LF@len\n"\
"JUMPIFEQ ord$error LF@cond bool@false\n"\
"STRI2INT LF@i LF@s LF@i\n"\
"PUSHS int@0\n"\
"PUSHS LF@i\n"\
"POPFRAME\n"\
"RETURN\n"\
"LABEL ord$error\n"\
"PUSHS int@1\n"\
"PUSHS int@0\n"\
"POPFRAME\n"\
"RETURN"

#define BUILTIN_chr "LABEL chr\n"\
"CREATEFRAME\n"\
"PUSHFRAME\n"\
"DEFVAR LF@i\n"\
"POPS LF@i\n"\
"DEFVAR LF@s\n"\
"DEFVAR LF@cond\n"\
"LT LF@cond LF@i int@0\n"\
"JUMPIFEQ chr$error LF@cond bool@true\n"\
"GT LF@cond LF@i int@255\n"\
"JUMPIFEQ chr$error LF@cond bool@true\n"\
"INT2CHAR LF@s LF@i\n"\
"PUSHS int@0\n"\
"PUSHS LF@s\n"\
"POPFRAME\n"\
"RETURN\n"\
"LABEL chr$error\n"\
"PUSHS int@1\n"\
"PUSHS string@\n"\
"POPFRAME\n"\
"RETURN"

#endif //IFJ_PROJEKT_BUILTIN_H
