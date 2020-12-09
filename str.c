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

/**
 * @brief Initialize a string
 *
 * @param s String to initialize
 * @return Exit code (0 = success)
 */
int strInit(string *s)
{
   if ((s->str = (char*) malloc(STR_LEN_INC)) == NULL)
      return STR_ERROR;
   s->str[0] = '\0';
   s->length = 0;
   s->allocSize = STR_LEN_INC;
   return STR_SUCCESS;
}

/**
 * @brief Initialize a string and copy the content of another string
 *
 * @param s1 String to initialize
 * @param s2 String to copy
 * @return Exit code (0 = success)
 */
int strInitCopy(string *s1, string *s2)
{
   return strInitFromConst(s1, s2->str);
}

/**
 * @brief Initialize a string and copy a c-string into it
 *
 * @param s1 String to initialize
 * @param s2 String to copy
 * @return Exit code (0 = success)
 */
int strInitFromConst(string *s1, const char *s2)
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

/**
 * @brief Initialize a string and copy a formatted string into it
 *
 * @param s String to initialize
 * @param fmt Formatting string
 * @param ... Format variables
 * @return Exit code (0 = success)
 */
int strInitFromFormat(string *s, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char buffer[FORMAT_BUFFER_LENGTH];
    vsnprintf(buffer, FORMAT_BUFFER_LENGTH, fmt, args);
    va_end(args);
    return strInitFromConst(s, buffer);
}

/**
 * @brief Quickly create a string with content of a c-string
 *
 * @param s String to copy
 * @return Created string
 */
string *strCreate(const char *s)
{
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

/**
 * @brief Quickly create a copy of a string
 *
 * @param s String to copy
 * @return Created string
 */
string *strCreateCopy(string *s)
{
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

/**
 * @brief Quickly create a string from a format
 *
 * @param fmt Formatting string
 * @param ... Format variables
 * @return Created string
 */
string *strCreateFromFormat(const char *fmt, ...)
{
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

/**
 * @brief Free string content
 *
 * @param s The string
 */
void strFree(string *s)
{
    if(!s || !s->str)
        return;
    free(s->str);
    s->str = NULL;
}

/**
 * @brief Clear a string's content
 *
 * @param s The string
 */
void strClear(string *s)
{
   s->str[0] = '\0';
   s->length = 0;
}

/**
 * @brief Append a single character to a string
 *
 * @param s1 The string
 * @param c Char to append
 * @return Exit code (0 = success)
 */
int strAddChar(string *s1, char c)
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

/**
 * @brief Copy the content of one string into another
 *
 * @param s1 The string to copy into
 * @param s2 The string to copy from
 * @return Exit code (0 = success)
 */
int strCopyString(string *s1, string *s2)
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

/**
 * @brief Concatenates a string after another
 *
 * @param s1 The string to concatenate to
 * @param s2 The string to concatenate from
 * @return Exit code (0 = success)
 */
int strConcatString(string *s1, string *s2)
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

/**
 * @brief Compare two strings
 *
 * @param s1 First string
 * @param s2 Second string
 * @return Identical to strcmp output (0 -> strings match)
 */
int strCmpString(string *s1, string *s2)
{
   return strcmp(s1->str, s2->str);
}

/**
 * @brief Compare a string to a c-string
 *
 * @param s1 The string
 * @param s2 The c-string
 * @return Identical to strcmp output (0 -> strings match)
 */
int strCmpConstStr(string *s1, char* s2)
{
   return strcmp(s1->str, s2);
}

/**
 * @brief Get the c-string content of a string
 *
 * @param s The string
 * @return Raw string content
 */
char *strGetStr(string *s)
{
   return s->str;
}

/**
 * @brief Get string length
 *
 * @param s The string
 * @return String length
 */
int strGetLength(string *s)
{
   return s->length;
}
