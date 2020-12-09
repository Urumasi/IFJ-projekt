/**
 * IFJ Project 2020: Team 008, variant II
 *
 * @file scanner.c
 *
 * @brief Scanner implementation
 *
 *
 * @author Martin Kneslík <xknesl02@stud.fit.vutbr.cz>
 * @author Adriana Jurkechová <xjurke02@stud.fit.vutbr.cz>
 * @author Karel Norek <xnorek01@stud.fit.vutbr.cz>
 * @author Petr Salaba <xsalab00@stud.fit.vutbr.cz>
 */

#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>

#include "scanner.h"

#define SCANNER_STATE_START 1                   // Starting state.
#define SCANNER_STATE_EOL 2                     // End of line.
#define SCANNER_STATE_KEYWORD_OR_IDENTIFIER 3   // Starts with underscore or alpha, later compared to keywords and executed as keyword or identifier.
#define SCANNER_STATE_NUMBER 4                  // Starts processing number.
#define SCANNER_STATE_DECIMAL_POINT 5           // If decimal point was detected, token type is float.
#define SCANNER_STATE_FLOAT 6                   // Processing float number.
#define SCANNER_STATE_EXPONENT 7                // If 'e' or 'E' was detected, token type is float.
#define SCANNER_STATE_EXPONENT_SIGNED 8         // Detected wheter there is '+' or '-' after exponent.
#define SCANNER_STATE_FINAL_NUMBER 9            // The final number with exponent.
#define SCANNER_STATE_STRING 10                 // Starts processing string.
#define SCANNER_STATE_ESCAPE 11                 // If '/' was detected, scanner is executing escape sequence.
#define SCANNER_STATE_LESS_THAN 12              // Executing wheter there is '=' after '<'.
#define SCANNER_STATE_MORE_THAN 13              // Executing wheter there is '=' sfter '>'
#define SCANNER_STATE_COMMENTARY 14             // Line commentary that is ignored.
#define SCANNER_STATE_BLOCK_COMMENTARY_START 15 // Start of block commentary, reads until sequence '*/' is found.
#define SCANNER_STATE_BLOCK_COMMENTARY_EXIT 16  // Character '/' has been found, ignores everything in the block.
#define SCANNER_STATE_ASSIGN 17                 // Executing wheter there is only one '=' in the row.
#define SCANNER_STATE_EQUAL 18                  // Executing wheter there are two '=' in the row.
#define SCANNER_STATE_NOT_EQUAL 19              // Executing wheter there is '=' after '!'.
#define SCANNER_STATE_SLASH 20                  // Executing wheter read character is division or commentary.
#define SCANNER_STATE_VARIABLE_DEF 21           // Executing wheter there is '=' after ':'.
#define SCANNER_STATE_CHARACTER 22              // Character a in sequence '/xab'.
#define SCANNER_STATE_CHARACTER_SECOND 23       // Character b in sequence '/xab'.
#define SCANNER_STATE_ZERO_CONTROL 24           // Controls leading zeros.

FILE *sourceFile;
string *setStr;

/**
 * @brief Sets source file.
 * @param f File that will be set as a source file.
 */
void setSourceFile(FILE *f)
{
    sourceFile = f;
}

/**
 * @brief Sets string.
 * @param s String that will be set as an attribute.
 */
void setString(string *s)
{
    setStr = s;
}

/**
 * @brief Frees resources and returns exit code.
 * @param exitCode Exit code
 * @param s String that will be freed.
 * @return Returns exit code.
 */
static int freeResources(int exitCode, string *s)
{
    strFree(s);
    return exitCode;
}

/**
 * @brief Identifies keywords or identifiers.
 * @param str String that will be compared to keywords and assigned as a keyword or an identifiers.
 * @param token Token whose string attribute will be set.
 * @return Returns exit code.
 */
static int processingKeywordIdentifier(string *str, Token *token)
{
    if (strCmpConstStr(str, "else") == 0)
        token->attribute.keyword = KW_ELSE;
    else if (strCmpConstStr(str, "float64") == 0)
        token->attribute.keyword = KW_FLOAT64;
    else if (strCmpConstStr(str, "for") == 0)
        token->attribute.keyword = KW_FOR;
    else if (strCmpConstStr(str, "func") == 0)
        token->attribute.keyword = KW_FUNC;
    else if (strCmpConstStr(str, "if") == 0)
        token->attribute.keyword = KW_IF;
    else if (strCmpConstStr(str, "int") == 0)
        token->attribute.keyword = KW_INT;
    else if (strCmpConstStr(str, "package") == 0)
        token->attribute.keyword = KW_PACKAGE;
    else if (strCmpConstStr(str, "return") == 0)
        token->attribute.keyword = KW_RETURN;
    else if (strCmpConstStr(str, "string") == 0)
        token->attribute.keyword = KW_STRING;
    else
        token->type = TOKEN_IDENTIFIER;

    if (token->type != TOKEN_IDENTIFIER)
    {
        token->type = TOKEN_KEYWORD;
        return freeResources(ERROR_CODE_OK, str);
    }

    if (strCopyString(token->attribute.string, str) == 1)
    {
        return freeResources(ERROR_INTERNAL, str);
    }

    return freeResources(ERROR_CODE_OK, str);
}

/**
 * @brief Processes integers.
 * @param token Token whose integer attribute will be set.
 * @param str String that will be converted to integer.
 * @return Returns exit code.
 */
static int proscessingInteger(Token *token, string *str)
{
    char *endptr;

    int64_t value = (int)strtoll(str->str, &endptr, 10);
    if (*endptr)
    {
        return freeResources(ERROR_INTERNAL, str);
    }
    token->attribute.integer = value;
    token->type = TOKEN_INT;
    return freeResources(ERROR_CODE_OK, str);
}

/**
 * @brief Processes decimals.
 * @param token Token whose decimal attribute will be set.
 * @param str String that will be converted to float.
 * @return Returns exit code.
 */
static int processingDecimal(Token *token, string *str)
{
    char *endptr;

    double value = strtod(str->str, &endptr);
    if (*endptr)
    {
        return freeResources(ERROR_CODE_OK, str);
    }
    token->attribute.decimal = value;
    token->type = TOKEN_FLOAT;
    return freeResources(ERROR_CODE_OK, str);
}

/**
 * @brief Produces token.
 * @param token Token whose attribute and type will be set.
 * @return Returns exit code.
 */
int getNextToken(Token *token)
{

    if (sourceFile == NULL)
    {
        return ERROR_INTERNAL;
    }

    token->attribute.string = setStr;

    string stringstr;
    string *str = &stringstr;
    if (strInit(str) == 1)
    {
        return ERROR_INTERNAL;
    }

    int state = SCANNER_STATE_START;
    token->type = TOKEN_NOTHING;

    char c, character[2] = {0};

    while (true)
    {
        c = (char)getc(sourceFile);

        switch (state)
        {
        case (SCANNER_STATE_START):
            if (c == '\n')
            {
                state = SCANNER_STATE_EOL;
            }
            else if (isspace(c))
            {
                state = SCANNER_STATE_START;
            }
            else if (c == '+')
            {
                token->type = TOKEN_PLUS;
                return freeResources(ERROR_CODE_OK, str);
            }
            else if (c == '-')
            {
                token->type = TOKEN_MINUS;
                return freeResources(ERROR_CODE_OK, str);
            }
            else if (c == '*')
            {
                token->type = TOKEN_MUL;
                return freeResources(ERROR_CODE_OK, str);
            }
            else if (c == '/')
            {
                state = SCANNER_STATE_SLASH;
            }
            else if (c == '<')
            {
                state = SCANNER_STATE_LESS_THAN;
            }
            else if (c == '>')
            {
                state = SCANNER_STATE_MORE_THAN;
            }
            else if (c == '=')
            {
                state = SCANNER_STATE_EQUAL;
            }
            else if (c == '!')
            {
                state = SCANNER_STATE_NOT_EQUAL;
            }
            else if (c == '(')
            {
                token->type = TOKEN_LBRACKET;
                return freeResources(ERROR_CODE_OK, str);
            }
            else if (c == ')')
            {
                token->type = TOKEN_RBRACKET;
                return freeResources(ERROR_CODE_OK, str);
            }
            else if (c == '{')
            {
                token->type = TOKEN_LCURLYBRACKET;
                return freeResources(ERROR_CODE_OK, str);
            }
            else if (c == '}')
            {
                token->type = TOKEN_RCURLYBRACKET;
                return freeResources(ERROR_CODE_OK, str);
            }
            else if (c == ',')
            {
                token->type = TOKEN_COMMA;
                return freeResources(ERROR_CODE_OK, str);
            }
            else if (c == ';')
            {
                token->type = TOKEN_SEMICOLON;
                return freeResources(ERROR_CODE_OK, str);
            }
            else if (c == ':')
            {
                state = SCANNER_STATE_VARIABLE_DEF;
            }
            else if (isalpha(c) || c == '_')
            {
                if (strAddChar(str, (char)c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
                state = SCANNER_STATE_KEYWORD_OR_IDENTIFIER;
            }
            else if (c == '0')
            {
                state = SCANNER_STATE_ZERO_CONTROL;
            }
            else if (isdigit(c))
            {
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
                state = SCANNER_STATE_NUMBER;
            }
            else if (c == '"')
            {
                state = SCANNER_STATE_STRING;
            }
            else if (c == EOF)
            {
                token->type = TOKEN_EOF;
                return freeResources(ERROR_CODE_OK, str);
            }
            else
            {
                return freeResources(ERROR_LEX, str);
            }
            break;

        case (SCANNER_STATE_LESS_THAN):
            if (c == '=')
            {
                token->type = TOKEN_LEQ;
            }
            else
            {
                ungetc(c, sourceFile);
                token->type = TOKEN_LT;
            }
            return freeResources(ERROR_CODE_OK, str);

        case (SCANNER_STATE_MORE_THAN):
            if (c == '=')
            {
                token->type = TOKEN_GEQ;
            }
            else
            {
                ungetc(c, sourceFile);
                token->type = TOKEN_GT;
            }
            return freeResources(ERROR_CODE_OK, str);

        case (SCANNER_STATE_EQUAL):
            if (c == '=')
            {
                token->type = TOKEN_EQ;
            }
            else
            {
                ungetc(c, sourceFile);
                token->type = TOKEN_ASSIGN;
            }
            return freeResources(ERROR_CODE_OK, str);

        case (SCANNER_STATE_NOT_EQUAL):
            if (c == '=')
            {
                token->type = TOKEN_NEQ;
                return freeResources(ERROR_CODE_OK, str);
            }
            else
            {
                ungetc(c, sourceFile);
                return freeResources(ERROR_LEX, str);
            }
            break;

        case (SCANNER_STATE_VARIABLE_DEF):
            if (c == '=')
            {
                token->type = TOKEN_VAR_DEF;
                return freeResources(ERROR_CODE_OK, str);
            }
            else
            {
                ungetc(c, sourceFile);
                return freeResources(ERROR_LEX, str);
            }
            break;

        case (SCANNER_STATE_KEYWORD_OR_IDENTIFIER):
            if (isalnum(c) || c == '_')
            {
                if (strAddChar(str, (char)c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else
            {
                ungetc(c, sourceFile);
                return processingKeywordIdentifier(str, token);
            }
            break;

        case (SCANNER_STATE_ZERO_CONTROL):
            if (isdigit(c) != 0)
            {
                return freeResources(ERROR_LEX, str);
            }
            else
            {
                ungetc(c, sourceFile);
                state = SCANNER_STATE_NUMBER;
            }
            break;

        case (SCANNER_STATE_NUMBER):
            if (isdigit(c))
            {
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else if (c == '.')
            {
                state = SCANNER_STATE_DECIMAL_POINT;
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else if (tolower(c) == 'e')
            {
                state = SCANNER_STATE_EXPONENT;
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else
            {
                ungetc(c, sourceFile);
                return proscessingInteger(token, str);
            }
            break;

        case (SCANNER_STATE_DECIMAL_POINT):
            if (isdigit(c))
            {
                state = SCANNER_STATE_FLOAT;
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else
            {
                return freeResources(ERROR_LEX, str);
            }
            break;

        case (SCANNER_STATE_FLOAT):
            if (isdigit(c))
            {
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else if (tolower(c) == 'e')
            {
                state = SCANNER_STATE_EXPONENT;
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else
            {
                ungetc(c, sourceFile);
                return processingDecimal(token, str);
            }
            break;

        case (SCANNER_STATE_EXPONENT):
            if (isdigit(c))
            {
                state = SCANNER_STATE_FINAL_NUMBER;
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else if (c == '+' || c == '-')
            {
                state = SCANNER_STATE_EXPONENT_SIGNED;
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else
            {
                return freeResources(ERROR_LEX, str);
            }
            break;

        case (SCANNER_STATE_EXPONENT_SIGNED):
            if (isdigit(c))
            {
                state = SCANNER_STATE_FINAL_NUMBER;
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else
            {
                return freeResources(ERROR_LEX, str);
            }
            break;

        case (SCANNER_STATE_FINAL_NUMBER):
            if (isdigit(c))
            {
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            else
            {
                ungetc(c, sourceFile);
                return processingDecimal(token, str);
            }
            break;

        case (SCANNER_STATE_STRING):
            if (c < ' ')
            {
                return freeResources(ERROR_LEX, str);
            }
            else if (c == '"')
            {
                if (strCopyString(token->attribute.string, str) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
                token->type = TOKEN_STRING;
                return freeResources(ERROR_CODE_OK, str);
            }
            else if (c == '\\')
            {
                state = SCANNER_STATE_ESCAPE;
            }
            else
            {
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
            }
            break;

        case (SCANNER_STATE_ESCAPE):
            if (c <= '!')
            {
                return freeResources(ERROR_LEX, str);
            }
            else if (c == '"')
            {
                c = '"';
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
                state = SCANNER_STATE_STRING;
            }
            else if (c == 'n')
            {
                c = '\n';
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
                state = SCANNER_STATE_STRING;
            }
            else if (c == 't')
            {
                c = '\t';
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
                state = SCANNER_STATE_STRING;
            }
            else if (c == '\\')
            {
                c = '\\';
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
                state = SCANNER_STATE_STRING;
            }
            else if (c == 'x')
            {
                state = SCANNER_STATE_CHARACTER;
            }
            else
            {
                return freeResources(ERROR_LEX, str);
            }
            break;

        case (SCANNER_STATE_CHARACTER):
            if (c >= '0' && c <= '9')
            {
                character[0] = c;
                state = SCANNER_STATE_CHARACTER_SECOND;
            }
            else if (c >= 'A' && c < 'F')
            {
                character[0] = c;
                state = SCANNER_STATE_CHARACTER_SECOND;
            }
            else if (c >= 'a' && c <= 'f')
            {
                character[0] = c;
                state = SCANNER_STATE_CHARACTER_SECOND;
            }
            else
            {
                return freeResources(ERROR_LEX, str);
            }
            break;

        case (SCANNER_STATE_CHARACTER_SECOND):
            if (c >= '0' && c <= '9')
            {
                character[1] = c;
                int val = (int)strtol(character, NULL, 16);
                c = (char)val;
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
                state = SCANNER_STATE_STRING;
            }
            else if (c >= 'A' && c <= 'F')
            {
                character[1] = c;
                int val = (int)strtol(character, NULL, 16);
                c = (char)val;
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
                state = SCANNER_STATE_STRING;
            }
            else if (c >= 'a' && c <= 'f')
            {
                character[1] = c;
                int val = (int)strtol(character, NULL, 16);
                c = (char)val;
                if (strAddChar(str, c) == 1)
                {
                    return freeResources(ERROR_INTERNAL, str);
                }
                state = SCANNER_STATE_STRING;
            }
            else
            {
                return freeResources(ERROR_LEX, str);
            }
            break;

        case (SCANNER_STATE_SLASH):
            if (c == '/')
            {
                state = SCANNER_STATE_COMMENTARY;
            }
            else if (c == '*')
            {
                state = SCANNER_STATE_BLOCK_COMMENTARY_START;
            }
            else
            {
                ungetc(c, sourceFile);
                token->type = TOKEN_DIV;
                return freeResources(ERROR_CODE_OK, str);
            }
            break;

        case (SCANNER_STATE_COMMENTARY):
            while (c != '\n')
            {
                c = (char)getc(sourceFile);
                if (c == EOF)
                {
                    state = SCANNER_STATE_START;
                    ungetc(c, sourceFile);
                    break;
                }
            }
            if (c == '\n')
            {
                state = SCANNER_STATE_START;
                ungetc(c, sourceFile);
            }
            break;

        case (SCANNER_STATE_BLOCK_COMMENTARY_START):
            if (c == '*')
            {
                state = SCANNER_STATE_BLOCK_COMMENTARY_EXIT;
            }
            else if (c == EOF)
            {
                return freeResources(ERROR_LEX, str);
            }
            break;

        case (SCANNER_STATE_BLOCK_COMMENTARY_EXIT):
            if (c == EOF)
            {
                return freeResources(ERROR_LEX, str);
            }
            else if (c == '/')
            {
                state = SCANNER_STATE_START;
            }
            else if (c == '*')
            {
                state = SCANNER_STATE_BLOCK_COMMENTARY_EXIT;
            }
            else
            {
                state = SCANNER_STATE_BLOCK_COMMENTARY_START;
            }
            break;

        case (SCANNER_STATE_EOL):
            if (isspace(c))
            {
                break;
            }
            ungetc(c, sourceFile);
            token->type = TOKEN_EOL;
            return freeResources(ERROR_CODE_OK, str);

        } //switch
    }     //while
} //funkcia
