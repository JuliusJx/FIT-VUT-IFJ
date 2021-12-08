/*
 * File: exp_parser.h
 * Header file for exp_parser.c
 * Subject: IFJ
 * Project: Implementace prekladace imperativniho jazyka IFJ21
 * Authors: Milan Hrabovský (xhrabo15), Ondrej Kovac (xkovac57), Jakub Julius Smykal (xsmyka01), Martin Talajka (xtalaj00)
 * Year: 2021
 */

#ifndef exp_parser_h_
#define exp_parser_h_

#include <stdio.h>
#include <stdbool.h>
#include "parser.h"
#include "str_stack.h"

#define STR_LEN 0       //| #
#define MUL 1           //| *
#define DIV 2           //| /
#define INT_DIV 3       //| //
#define PLUS 4          //| +
#define MINUS 5         //| -
#define STR_CONC 6      //| ..
#define R_EQ 7          //| ==
#define R_NEQ 8         //| ~=
#define R_LE 9          //| <
#define R_GR 10         //| >
#define R_LEQ 11        //| <=
#define R_GRQ 12        //| >=
#define LEFT_PAR 13     //| (
#define RIGHT_PAR 14    //| )
#define T_INT 15        //| i -> int
#define T_NUM 16        //| i -> float
#define T_STR 17        //| i -> string
#define T_INT_V 18      //| i -> int (variable)
#define T_NUM_V 19      //| i -> float (variable)
#define T_STR_V 20      //| i -> string (variable)
#define T_DOLLAR 21     //| $
#define T_NIL 22        //| nil
#define T_INT_V_NIL 23  //| variable int nill
#define T_NUM_V_NIL 24  //| variable str nill
#define T_STR_V_NIL 25  //| variable str nill
#define T_BOOL 26       //| bool type

#define LE 45            // <
#define GR 44            // >
#define EQ 43            // =
#define ER 42            // ERROR

#define PREC_TAB_SIZE 26

int tokConversion( token*, s_stack*);

bool phCheck( stack*, s_stack*, int, int*, int);

bool pHelp( stack*, s_stack*, int);

bool pAlgo( stack*, s_stack*, int);

bool pExpression( int);

bool pConvertFloat( contentInput*, char*,  char*, int, int);

void cClean( stack*, s_stack*);

#endif

// --End of file--