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
#define LEX_ERROR		1 // Lexical error
#define SYN_ERROR		2 // Syntactic error
#define SEM_ERROR		3 // Semantic error - undefined variable, redefinition of a function, etc.
#define SEM_TYPE_ERROR	4 // Semantic error - error while deducing type of new defined variable. 
#define SEM_COMP_ERROR	5 // Semantic error - error of type compability.
#define SEM_PARAM_ERROR	6 // Semantic error - wrong parameters, wrong return type.
#define SEM_OTHER_ERROR	7 // Semantic error - other.
#define DIV_ZERO_ERROR	9 // Divison by zero.
#define INTERNAL_ERROR	99 // Internal error of compiler (eg. failed allocation of memory).

#endif //__ERROR_H__
