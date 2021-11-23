/*
 * File: parser.h
 * Header file for parser.c
 * Subject: IFJ
 * Authors: Jakub Julius Smykal, Ondrej Kovac, Martin Talajka, Milan Hrabovsky
 * Year: 2021
 */

#ifndef parser_h_
#define parser_h_

#include <stdio.h>
#include <stdbool.h>
#include "scanner.h"
#include "symtable.h"
#include "stack.h"

#define P_PARAM 0
#define P_RETURN 1
#define P_ARG 2
#define P_STATEMENT 3

int main();

bool cmpTokType( token*, int);

token *nextToken();

void freeToken( token*);

bool pProgram();

bool pBody();

bool pCall();

bool pParams();

bool pParam();

bool pReturns();

bool pArgs();

bool pArg();

bool pCallArgs();

bool pStatement();

bool pAfter_ID();

bool pAfter_EQ();

bool pInit();

bool pID( bool);

bool pType();

#endif

// --End of file--