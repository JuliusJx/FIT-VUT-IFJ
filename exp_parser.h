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

#define STR_LEN 0       //| #
#define MUL_DIV 1       //| *, / 
#define MOD 2           //| //
#define PLUS_MINUS 3    //| +, -
#define STR_CONC 4      //| ..
#define REL_COMP 5      //| ==, ~=, <, >, <=, >=
#define LEFT_PAR 6      //| (
#define RIGHT_PAR 7     //| )
#define T_INT 8         //| i -> int (variable)
#define T_NUM 9         //| i -> float (variable)
#define T_STRING 10     //| i -> string (variable)
#define T_DOLLAR 11     //| $

#define LE 1            // <
#define GR 2            // >
#define EQ 3            // =
#define ER 0            // ERROR

#define PREC_TAB_SIZE 9

#endif

// --End of file--