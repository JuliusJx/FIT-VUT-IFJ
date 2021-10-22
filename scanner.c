/*
 * File: scanner.c
 * c file for lexical analysis
 * Subject: IFJ
 * Authors: Jakub Julius Smykal
 * Year: 2021
 */

#include "stdio.h"
#include "scanner.h"

token *GetToken(){
    token *NewToken;
    unsigned FSM_State = FSM_Start;
    char c;
    short done = 0;

    while(!done){
        c = getc(stdin);
        switch (FSM_State){
            case FSM_Start:

        }
    }
    return NewToken;
}

// --End of file--