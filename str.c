//prevzaný súbor zo stránky predmetu IFJ

//jednoducha knihovna pro praci s nekonecne dlouhymi retezci
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include "str.h"

#define STR_LEN_INC 8
// konstanta STR_LEN_INC udava, na kolik bytu provedeme pocatecni alokaci pameti
// pokud nacitame retezec znak po znaku, pamet se postupne bude alkokovat na
// nasobky tohoto cisla 

#define STR_ERROR   1
#define STR_SUCCESS 0

int strInit(string *s)
// funkce vytvori novy retezec
{
   if ((s->str = (char*) malloc(STR_LEN_INC)) == NULL)
      return STR_ERROR;
   s->str[0] = '\0';
   s->length = 0;
   s->allocSize = STR_LEN_INC;
   return STR_SUCCESS;
}

int strInitCopy(string *s1, string *s2)
// kopirovaci konstruktor
{
   return strInitFromConst(s1, s2->str);
}

int strInitFromConst(string *s1, const char *s2)
// funkce vytvori novy retezec s obsahem s2
{
   if(!s1 || !s2)
       return STR_ERROR;
   if ((s1->str = (char*) malloc(strlen(s2)+1)) == NULL)
       return STR_ERROR;
   strcpy(s1->str, s2);
   s1->length = strlen(s2);
   s1->allocSize = s1->length + 1;
   return STR_SUCCESS;
}

int strInitFromFormat(string *s, const char *fmt, ...){
    va_list args;
    va_start(args, fmt);
    char buffer[FORMAT_BUFFER_LENGTH];
    vsnprintf(buffer, FORMAT_BUFFER_LENGTH, fmt, args);
    va_end(args);
    return strInitFromConst(s, buffer);
}

string *strCreate(const char *s){
    string *out = malloc(sizeof(string));
    if(!out)
        return NULL;
    int ret = strInitFromConst(out, s);
    if(ret){
        free(out);
        return NULL;
    }
    return out;
}

string *strCreateCopy(string *s){
    string *out = malloc(sizeof(string));
    if(!out)
        return NULL;
    int ret = strInitFromConst(out, s->str);
    if(ret){
        free(out);
        return NULL;
    }
    return out;
}

string *strCreateFromFormat(const char *fmt, ...){
    string *out = malloc(sizeof(string));
    if(!out)
        return NULL;
    va_list args;
    va_start(args, fmt);
    char buffer[FORMAT_BUFFER_LENGTH];
    vsnprintf(buffer, FORMAT_BUFFER_LENGTH, fmt, args);
    va_end(args);
    int ret = strInitFromConst(out, buffer);
    if(ret){
        free(out);
        return NULL;
    }
    return out;
}

void strFree(string *s)
// funkce uvolni retezec z pameti
{
    if(!s || !s->str)
        return;
    free(s->str);
    s->str = NULL;
}

void strClear(string *s)
// funkce vymaze obsah retezce
{
   s->str[0] = '\0';
   s->length = 0;
}

int strAddChar(string *s1, char c)
// prida na konec retezce jeden znak
{
   if (s1->length + 1 >= s1->allocSize)
   {
      // pamet nestaci, je potreba provest realokaci
      if ((s1->str = (char*) realloc(s1->str, s1->length + STR_LEN_INC)) == NULL)
         return STR_ERROR;
      s1->allocSize = s1->length + STR_LEN_INC;
   }
   s1->str[s1->length] = c;
   s1->length++;
   s1->str[s1->length] = '\0';
   return STR_SUCCESS;
}

int strCopyString(string *s1, string *s2)
// prekopiruje retezec s2 do s1
{
   int newLength = s2->length;
   if (newLength >= s1->allocSize)
   {
      // pamet nestaci, je potreba provest realokaci
      if ((s1->str = (char*) realloc(s1->str, newLength + 1)) == NULL)
         return STR_ERROR;
      s1->allocSize = newLength + 1;
   }
   strcpy(s1->str, s2->str);
   s1->length = newLength;
   return STR_SUCCESS;
}

int strConcatString(string *s1, string *s2)
// prida retezec s2 za retezec s1
{
   int newLength = s1->length + s2->length;
   if (newLength >= s1->allocSize)
   {
      // pamet nestaci, je potreba provest realokaci
      if ((s1->str = (char*) realloc(s1->str, newLength + 1)) == NULL)
         return STR_ERROR;
      s1->allocSize = newLength + 1;
   }
   strcat(s1->str, s2->str);
   s1->length = newLength;
   return STR_SUCCESS;
}

int strCmpString(string *s1, string *s2)
// porovna oba retezce a vrati vysledek
{
   return strcmp(s1->str, s2->str);
}

int strCmpConstStr(string *s1, char* s2)
// porovna nas retezec s konstantnim retezcem
{
   return strcmp(s1->str, s2);
}

char *strGetStr(string *s)
// vrati textovou cast retezce
{
   return s->str;
}

int strGetLength(string *s)
// vrati delku daneho retezce
{
   return s->length;
}
