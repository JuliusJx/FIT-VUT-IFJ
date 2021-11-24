/*
 * File: exp_parser.c
 * c file bottom up analysis
 * Subject: IFJ
 * Authors: Jakub Julius Smykal, Ondrej Kovac, Martin Talajka, Milan Hrabovsky
 * Year: 2021
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "exp_parser.h"

int prec_table[PREC_TAB_SIZE][PREC_TAB_SIZE] = {
 // | #      | *,/,// | +,-   | ..    | Rel   | (     | )     | i     | $
    { EQ,       GR,     GR,     GR,     GR,     GR,     GR,     GR,     GR },    //| #
    { LE,       GR,     GR,     GR,     GR,     LE,     GR,     LE,     GR },    //| *, /, //
    { LE,       LE,     GR,     GR,     GR,     LE,     GR,     LE,     GR },    //| +, -
    { LE,       LE,     LE,     LE,     LE,     LE,     GR,     LE,     GR },    //| ..
    { LE,       LE,     LE,     GR,     ER,     LE,     GR,     LE,     GR },    //| Rel
    { LE,       LE,     LE,     LE,     LE,     LE,     EQ,     LE,     ER },    //| (
    { LE,       GR,     GR,     GR,     GR,     ER,     GR,     ER,     GR },    //| )
    { LE,       GR,     GR,     GR,     GR,     ER,     GR,     ER,     GR },    //| i
    { LE,       LE,     LE,     LE,     LE,     LE,     ER,     LE,     ER },    //| $
};

int tokConversion(token *cToken){
    switch (cToken->type){
        case TOKEN_StrLen:
            return STR_LEN;
        case TOKEN_Mul:
        case TOKEN_Div:
            return MUL_DIV;
        case TOKEN_DivInt:
            return MOD;
        case TOKEN_Plus:
        case TOKEN_Minus:
            return PLUS_MINUS;
        case TOKEN_Concat:
            return STR_CONC;
        case TOKEN_Greater:
        case TOKEN_GreatEQ:
        case TOKEN_Less:
        case TOKEN_LessEQ:
        case TOKEN_EQ:
        case TOKEN_NotEQ:
            return REL_COMP;
        case TOKEN_LeftPar:
            return LEFT_PAR;
        case TOKEN_RightPar:
            return RIGHT_PAR;
        case TOKEN_Int:
            return T_INT;
        case TOKEN_Num:
            return T_NUM;
        case TOKEN_String:
            return T_STRING;
        case TOKEN_ID:
            tableItem *item = symGetItem(table, cToken->content.str, scope);
            if(item == NULL)
                return T_DOLLAR;
            if(item == TYPE_INT)
                return T_INT;
            if(item == TYPE_NUM)
                return T_NUM;
            if(item == TYPE_STR)
                return T_STRING;
        default:
            return T_DOLLAR;
    }
}

bool pExpression(){
    bool stringValid = true;

    while(stringValid){
        token *cToken;
        if((cToken = get_token()) == NULL){
            return false;   // TODO: Asi nejaké uvolenenie zásobníku ? idk čo všetko sa tu bude robiť ešte a error ?
        }

        if(cToken->type == TOKEN_ID){
            tableItem *item = symGetItem(table, cToken->content.str, scope);
            if(item == NULL){
                return false;   // TODO:
            }
            if(item->type == FUNC_ID){
                return false;   // TODO:
            }
        }



    }






    return true;
}

// --End of file--