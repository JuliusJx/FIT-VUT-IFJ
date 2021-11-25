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

void errPrint( int, token*);

#endif

// --End of file-