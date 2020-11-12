/**
 * IFJ Project 2020: Team 008, variant II
 * 
 * @file main.c
 * 
 * @brief
 *
 * 
 * @author Martin Kneslík <xknesl02@stud.fit.vutbr.cz>
 * @author Adriana Jurkechová <xjurke02@stud.fit.vutbr.cz>
 * @author Karel Norek <xnorek01@stud.fit.vutbr.cz>
 * @author Petr Salaba <xsalab00@stud.fit.vutbr.cz>
 */

#include "htab/htab.h"
#include "scanner.h"
#include "error.h"
#include "parser.h"

int main(int argc, char *argv[])
{
    FILE *fp;
    if (argc > 1)
    {
        fp = fopen(argv[1], "r");
        if (fp == NULL)
        {
            fprintf(stderr, "Not a file\n");
            return ERROR_INTERNAL;
        }
    }
    else
        fp = stdin;
    setSourceFile(fp);

    int error;
    error = parse();

    // fprintf(stderr, "Program returned %d\n", error);

    fclose(fp);
    return error;
}
