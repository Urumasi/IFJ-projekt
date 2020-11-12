#include <stdio.h>
#include "../src/scanner.h"

void printToken(Token a)
{
    switch(a.type){
        case 0:
            printf("EQ ");
            break;
        case 1:
            printf("NEQ ");
            break;
        case 2:
            printf("LEQ ");
            break;
        case 3:
            printf("GEQ ");
            break;
        case 4:
            printf("LT ");
            break;
        case 5:
            printf("GT ");
            break;
        case 6:
            printf("PLUS ");
            break;
        case 7:
            printf("MINUS ");
            break;
        case 8:
            printf("MUL ");
            break;
        case 9:
            printf("DIV ");
            break;
        case 10:
            printf("ASSIGN ");
            break;
        case 11:
            printf("VAR_DEF ");
            break;
        case 12:
            printf("COMMA ");
            break;
        case 13:
            printf("SEMICOLON ");
            break;
        case 14:
            printf("RBRACKET ");
            break;
        case 15:
            printf("LBRACKET ");
            break;
        case 16:
            printf("RCURLYBRACKET ");
            break;
        case 17:
            printf("LCURLYBRACKET ");
            break;
        case 18:
            printf("KEYWORD(");
            switch (a.attribute.keyword)
            {
            case 0:
                printf("ELSE");
                break;
            case 1:
                printf("FLOAT");
                break;
            case 2:
                printf("FOR");
                break;
            case 3:
                printf("FUNC");
                break;
            case 4:
                printf("IF");
                break;
            case 5:
                printf("INT");
                break;
            case 6:
                printf("PACKAGE");
                break;
            case 7:
                printf("RETURN");
                break;
            case 8:
                printf("STRING");
                break;
            }
            printf(") ");
            break;
        case 19:
            printf("IDENTIFIER(%s) ", a.attribute.string->str);
            break;
        case 20:
            printf("EOL\n");
            break;
        case 21:
            printf("EOF\n");
            break;
        case 22:
            printf("INT(%d) ", a.attribute.integer);
            break;
        case 23:
            printf("FLOAT(%f) ", a.attribute.decimal);
            break;
        case 24:
            printf("STRING(%s) ", a.attribute.string->str);
            break;
        case 25:
            printf("NOTHING ");
            break;
    }
}

int main(int argc, char *argv[]) {
    FILE *fp;
	
	if (argc != 2) {
		printf("Invalid number of arguments\n");
		return 1;
	}
	
    fp = fopen(argv[1], "r");
	if (fp == NULL) {
		printf("Couldn't open file %s\n", argv[1]);
		return 1;
	}

    setSourceFile(fp);
    Token a;
    while(getNextToken(&a)!=99) {
        printToken(a);
		if (a.type == TOKEN_EOF) {
			break;
		}
    }
    fclose(fp);
    return 0;
}
