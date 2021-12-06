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
#include "symstack.h"
#include "exp_parser.h"
#include "err_handler.h"
#include "generator.h"

//no idea what this is for
/*#define P_PARAM 0
#define P_RETURN 1
#define P_ARG 2
#define P_STATEMENT 3*/

extern token *returnToken;
extern symTable *table;
extern stack *argStack;
extern symstack *symStack;
extern char *tokenID; //maybe rename to functionID
extern int scope;
extern stack *blockStack;
extern bool isCondition;

bool cmpTokType( token*, int);

token *nextToken();

void freeToken( token*);

bool insertBuiltIn();

bool pProgram();

bool pBody();

bool pCall();

bool pParams();

bool pParam();

bool pReturns( bool);

bool pArgs();

bool pCallArgs();

bool pStatement();

bool pInit();

bool pID();

#endif

// --End of file--