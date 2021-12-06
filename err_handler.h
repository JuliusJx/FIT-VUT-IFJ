/*
 * File: err_handler.h
 * Header file for err_handler.c
 * Subject: IFJ
 * Authors: Jakub Julius Smykal, Ondrej Kovac
 * Year: 2021
 */

#ifndef errHandler_h_
#define errHandler_h_

#include <stdio.h>
#include "scanner.h"

extern int errCode;

#define ERR_CHECK(command,err_code,err_string) if(command){errPrint(err_code, cToken, err_string);freeToken(cToken);return false;}

void errPrint( int, token*, char*);

void printToken(token*);

#endif

// --End of file-