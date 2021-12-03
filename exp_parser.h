/*
 * File: exp_parser.h
 * Header file for exp_parser.c
 * Subject: IFJ
 * Authors: Jakub Julius Smykal, Ondrej Kovac, Martin Talajka, Milan Hrabovsky
 * Year: 2021
 */

#ifndef exp_parser_h_
#define exp_parser_h_

#include <stdio.h>
#include <stdbool.h>
#include "parser.h"

#define STR_LEN 0       //| #
#define MUL 1           //| *
#define DIV 2           //| / 
#define INT_DIV 3       //| //
#define PLUS_MINUS 4    //| +, -
#define STR_CONC 5      //| ..
#define REL_COMP_1 6    //| ==, ~=
#define REL_COMP_2 7    //| <, >, <=, >=
#define LEFT_PAR 8      //| (
#define RIGHT_PAR 9     //| )
#define T_INT 10        //| i -> int
#define T_NUM 11        //| i -> float
#define T_STR 12        //| i -> string
#define T_INT_V 13      //| i -> int (variable)
#define T_NUM_V 14      //| i -> float (variable)
#define T_STR_V 15      //| i -> string (variable)
#define T_DOLLAR 16     //| $
#define T_NIL 17        //| nil
#define T_INT_V_NIL 18  //| variable int nill
#define T_NUM_V_NIL 19  //| variable str nill
#define T_STR_V_NIL 20  //| variable str nill
#define T_BOOL 21       //| bool type

#define LE 45            // <
#define GR 44            // >
#define EQ 43            // =
#define ER 42            // ERROR

#define PREC_TAB_SIZE 21

int tokConversion( token*);

bool phCheck( stack*, stack*, int, int*, int);

bool pHelp( stack*, stack*, int);

bool pAlgo( stack*, stack*, int);

bool pExpression();

#endif

// --End of file--