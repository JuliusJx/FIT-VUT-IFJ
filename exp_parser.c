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
//  | 0  | 1   | 2  | 3   | 4  | 5   | 6 | 7 | 8      | 9      | 10     | 11
//  | #  | *,/ | // | +,- | .. | Rel | ( | ) | i(int) | i(num) | i(str) | $
    { EQ,  GR,   GR,  GR,   GR,  GR,  GR, GR,  GR,      GR,      GR,      GR },    //| #      | 0
    { LE,  GR,   GR,  GR,   GR,  GR,  LE, GR,  LE,      LE,      LE,      GR },    //| *, /   | 1
    { LE,  GR,   GR,  GR,   GR,  GR,  LE, GR,  LE,      LE,      LE,      GR },    //| //     | 2
    { LE,  LE,   LE,  GR,   GR,  GR,  LE, GR,  LE,      LE,      LE,      GR },    //| +, -   | 3
    { LE,  LE,   LE,  LE,   LE,  LE,  LE, GR,  LE,      LE,      LE,      GR },    //| ..     | 4
    { LE,  LE,   LE,  LE,   GR,  ER,  LE, GR,  LE,      LE,      LE,      GR },    //| Rel    | 5
    { LE,  LE,   LE,  LE,   LE,  LE,  LE, EQ,  LE,      LE,      LE,      ER },    //| (      | 6
    { LE,  GR,   GR,  GR,   GR,  GR,  ER, GR,  ER,      ER,      ER,      GR },    //| )      | 7
    { LE,  GR,   GR,  GR,   GR,  GR,  ER, GR,  ER,      ER,      ER,      GR },    //| i(int) | 8
    { LE,  GR,   GR,  GR,   GR,  GR,  ER, GR,  ER,      ER,      ER,      GR },    //| i(num) | 9
    { LE,  GR,   GR,  GR,   GR,  GR,  ER, GR,  ER,      ER,      ER,      GR },    //| i(str) | 10
    { LE,  LE,   LE,  LE,   LE,  LE,  LE, ER,  LE,      LE,      LE,      ER },    //| $      | 11
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

bool pHelp(stack *stack, int token){
    int tmp_top = 0; 
    int tmp = 0;
    // pop top (char) 
    stackPop(stack, &tmp_top);
    // - check na top == "<"
    stackTop(stack, &tmp);
    //  == pop
    if(tmp == LE)
        stackPop(stack, &tmp);
    //  =! error
    else
        printf("ERROR");
        return false;

    // check top < vstup
    //  == push(<), push(char), push(vstup)
    if(prec_table[tmp_top][token] == LE){
        stackPush(stack, tmp_top);
        if(token != T_DOLLAR)
            stackPush(stack, token);
    }
    //  =! ??
    return true;
}

bool pAlgo(stack *stack, int token){
    int tmp = 0;
    stackTop(stack, &tmp);

    if(token == T_DOLLAR){
        // TODO: chceck na to či je to error alebo koniec vstupného reťazcu
        int tmp_top = 0;
        int tmp_pop = 0;

        while(tmp_top != T_DOLLAR){
            if(!pHelp(stack, token)){
                printf("ERROR");
                return false;
            }

            stackPop(stack, &tmp_pop);
            stackTop(stack, &tmp_top);
            stackPush(stack, tmp_pop);
        }
        return true;
    }

    switch(prec_table[tmp][token]){
        case EQ:
            stackPush(stack, token);
            return true;
        case LE:
            stackPush(stack, LE);
            stackPush(stack, token);
            return true;
        case GR:
            if(pHelp(stack, token))
                return true;
            else{
                printf("ERROR");
                return false;
            }
        case ER:
            // TODO: ERROR print ?
            printf("ERROR");
            return false;
    }
}

bool pExpression(){
    bool stringValid = true;

    stack *stack = malloc(sizeof(stack));   //TODO: nezabudnúť uvoľniť
    stackInit(stack);
    stackPush(stack, T_DOLLAR);

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
        pAlgo(stack, tokConversion(cToken));

    }
    return true;
}

// --End of file--