/**
 * IFJ Project 2020: Team 008, variant II
 * 
 * @file error.h
 * 
 * @brief Definition of error codes.
 *
 * 
 * @author Martin Kneslík <xknesl02@stud.fit.vutbr.cz>
 * @author Adriana Jurkechová <xjurke02@stud.fit.vutbr.cz>
 * @author Karel Norek <xnorek01@stud.fit.vutbr.cz>
 * @author Petr Salaba <xsalab00@stud.fit.vutbr.cz>
 */

#ifndef __ERROR_H__
#define __ERROR_H__

#define ERROR_CODE_OK 	0 // Succesful return code
#define ERROR_LEX		1 // Lexical error
#define ERROR_SYN		2 // Syntactic error
#define ERROR_SEM		3 // Semantic error - undefined variable, redefinition of a function, etc.
#define ERROR_SEM_TYPE	4 // Semantic error - error while deducing type of new defined variable. 
#define ERROR_SEM_COMP	5 // Semantic error - error of type compability.
#define ERROR_SEM_PARAM	6 // Semantic error - wrong parameters, wrong return type.
#define ERROR_SEM_OTHER	7 // Semantic error - other.
#define ERROR_DIV_ZERO	9 // Divison by zero.
#define ERROR_INTERNAL	99 // Internal error of compiler (eg. failed allocation of memory).

#endif //__ERROR_H__
