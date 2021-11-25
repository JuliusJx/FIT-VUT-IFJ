/*
 * File: err_handler.c
 * c file for printing errors and setting error code
 * Subject: IFJ
 * Authors: Jakub Julius Smykal, Ondrej Kovac
 * Year: 2021
 */

#include <stdio.h>
#include "err_handler.h"

int errCode = 0;

void errPrint( int errType, token *token){
    errCode = errType;
    fprintf(stderr, "ERROR: -- code: %d, line: %d\n", errType, token->line);
}

// --End of file-