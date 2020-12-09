//prevzaný súbor zo stránky predmetu IFJ
#ifndef __STR_H__
#define __STR_H__

#include <stdarg.h>

#define FORMAT_BUFFER_LENGTH 256

//hlavickovy soubor pro praci s nekonecne dlouhymi retezci

typedef struct
{
  char *str;     // misto pro dany retezec ukonceny znakem '\0'
  int length;    // skutecna delka retezce
  int allocSize; // velikost alokovane pameti
} string;

int strInit(string *s);
int strInitCopy(string *s1, string *s2);
int strInitFromConst(string *s1, const char *s2);
int strInitFromFormat(string *s, const char *fmt, ...);
string *strCreate(const char *s);
string *strCreateCopy(string *s);
string *strCreateFromFormat(const char *fmt, ...);
void strFree(string *s);

void strClear(string *s);
int strAddChar(string *s1, char c);
int strCopyString(string *s1, string *s2);
int strConcatString(string *s1, string *s2);
int strCmpString(string *s1, string *s2);
int strCmpConstStr(string *s1, char *s2);

char *strGetStr(string *s);
int strGetLength(string *s);

#endif
