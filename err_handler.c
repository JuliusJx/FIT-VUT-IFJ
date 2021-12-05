/*
 * File: err_handler.c
 * c file for printing errors and setting error code
 * Subject: IFJ
 * Authors: Jakub Julius Smykal, Ondrej Kovac
 * Year: 2021
 */

#include <stdio.h>
#include "err_handler.h"
#include <string.h>

int errCode = 0;

void errPrint( int errType, token *token, char *err_string){
    errCode = errType;

    fprintf(stderr, "ERROR: Line: %d Type:", token->line);

    switch(errType){
        case 1:
            fprintf(stderr, "Lexical Error");
            break;
        case 2:
            fprintf(stderr, "Syntax Error");
            break;
        case 3:
            fprintf(stderr, "Semantic Error (definitions)");
            break;
        case 4:
            fprintf(stderr, "Semantic Error (bad type)");
            break;
        case 5:
            fprintf(stderr, "Semantic Error (args/returns)");
            break;
        case 6:
            fprintf(stderr, "Semantic Error (expression)");
            break;
        case 7:
            fprintf(stderr, "Semantic Error");
            break;
        case 8:
            fprintf(stderr, "Unexpected nil");
            break;
        case 9:
            fprintf(stderr, "Division by zero");
            break;
        case 99:
            fprintf(stderr, "Internal Compiler Error");
            break;
        default: break;
    
    }
    if (!strcmp(err_string, "bad_token"))
    {
        fprintf(stderr, "BAD_TOKEN");
    }

}

// --End of file-