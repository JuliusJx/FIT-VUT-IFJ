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

bool pStatement();

bool pAfter_ID();

bool pAfter_EQ();

bool pInit();

bool pID();

bool pType();

#endif

// --End of file--