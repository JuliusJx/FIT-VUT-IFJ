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
#include "exp_parser.h"

                // TOP          // VSTUP
int prec_table[PREC_TAB_SIZE][PREC_TAB_SIZE] = {
//  | 0  | 1  | 2  | 3 | 4   | 5  | 6   | 7 | 8 | 9      | 10      | 11    | 12
//  | #  | *  | /  |// | +,- | .. | Rel | ( | ) | i(int) | i(num) | i(str) | $
    { EQ,  GR,  GR, GR,  GR,   GR,  GR,  LE, GR,  GR,      GR,      GR,      GR },    //| #      | 0
    { LE,  GR,  GR, GR,  GR,   GR,  GR,  LE, GR,  LE,      LE,      LE,      GR },    //| *      | 1
    { LE,  GR,  GR, GR,  GR,   GR,  GR,  LE, GR,  LE,      LE,      LE,      GR },    //| /      | 2
    { LE,  GR,  GR, GR,  GR,   GR,  GR,  LE, GR,  LE,      LE,      LE,      GR },    //| //     | 3
    { LE,  LE,  LE, LE,  GR,   GR,  GR,  LE, GR,  LE,      LE,      LE,      GR },    //| +, -   | 4
    { LE,  LE,  LE, LE,  LE,   LE,  LE,  LE, GR,  LE,      LE,      LE,      GR },    //| ..     | 5
    { LE,  LE,  LE, LE,  LE,   GR,  ER,  LE, GR,  LE,      LE,      LE,      GR },    //| Rel    | 6
    { LE,  LE,  LE, LE,  LE,   LE,  LE,  LE, EQ,  LE,      LE,      LE,      ER },    //| (      | 7
    { LE,  GR,  GR, GR,  GR,   GR,  GR,  ER, GR,  ER,      ER,      ER,      GR },    //| )      | 8
    { LE,  GR,  GR, GR,  GR,   GR,  GR,  ER, GR,  ER,      ER,      ER,      GR },    //| i(int) | 9
    { LE,  GR,  GR, GR,  GR,   GR,  GR,  ER, GR,  ER,      ER,      ER,      GR },    //| i(num) | 10
    { LE,  GR,  GR, GR,  GR,   GR,  GR,  ER, GR,  ER,      ER,      ER,      GR },    //| i(str) | 11
    { LE,  LE,  LE, LE,  LE,   LE,  LE,  LE, ER,  LE,      LE,      LE,      ER },    //| $      | 12
};

int tokConversion(token *cToken){
    tableItem *item;
    switch (cToken->type){
        case TOKEN_StrLen:
            return STR_LEN;
        case TOKEN_Mul:
            return MUL;
        case TOKEN_Div:
            return DIV;
        case TOKEN_DivInt:
            return INT_DIV;
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
            return T_STR;
        case TOKEN_ID:
            item = symGetItem(table, cToken->content.str, scope);
            if(item == NULL)
                return T_DOLLAR;
            if(item->type == TYPE_INT)
                return T_INT;
            if(item->type == TYPE_NUM)
                return T_NUM;
            if(item->type == TYPE_STR)
                return T_STR;
        default:
            return T_DOLLAR;
    }
}

bool phCheck(stack *e_stack, stack *h_stack, int tmp_top, int *tmp_top2, int token){        // TODO: prepísať tie tmp_topy na niečo rozumnejšie
    stackTop(e_stack, tmp_top2);
    if(prec_table[*tmp_top2][token] == LE){
        stackPush(e_stack, LE);
        stackPush(e_stack, tmp_top);
        stackPush(e_stack, token);
        return true;
    }
    else{
        stackPush(e_stack, tmp_top);
        if(pHelp(e_stack, h_stack, token))
            return true;
        else
            return false;
    }
}

bool pHelp(stack *e_stack, stack *h_stack, int token){
    int tmp_top = 0;
    int tmp_top2 = 0;
    int tmp_pop = 0;
    int tmp_pop2 = 0;

    stackPop(e_stack, &tmp_pop);
    stackTop(e_stack, &tmp_top);

    if(tmp_top == LE){
        stackPop(e_stack, &tmp_top);
        stackTop(e_stack, &tmp_top);
        if(prec_table[tmp_top][token] == LE){
            stackPush(e_stack, LE);
            stackPush(e_stack, tmp_pop);
            stackPush(e_stack, token);
            return true;
        }
        else{
            stackPush(e_stack, tmp_pop);
            if(pHelp(e_stack, h_stack, token))
                return true;
            else{
                printf("ERROR-111\n");
                return false;
            }
        }
    }
    else{
        stackPop(e_stack, &tmp_pop2);
        stackPop(e_stack, &tmp_top);
        stackTop(e_stack, &tmp_top2);
        if(tmp_top2 == LE){
            stackPop(e_stack, &tmp_top2);
            // spojiť podľa pravidla
            // T_INT && MUL || DIV    && T_INT    = T_NUM;
            if( (tmp_top == T_INT) && (tmp_pop2 == MUL || tmp_pop2 == DIV) && (tmp_pop == T_INT) ){
                if(phCheck(e_stack, h_stack, T_NUM, &tmp_top2, token))
                    return true;
                else{
                    printf("ERROR-12\n");
                    return false;
                }
            }
            // T_NUM && PLUS_MINUS && T_INT    = T_NUM;
            else if ( (tmp_top == T_NUM) && (tmp_pop2 == PLUS_MINUS || tmp_pop2 == MUL || tmp_pop2 == DIV) && (tmp_pop == T_INT) ){
                if(phCheck(e_stack, h_stack, T_NUM, &tmp_top2, token))
                    return true;
                else{
                    printf("ERROR-12\n");
                    return false;
                }
            }
            // T_NUM || T_INT && MUL || DIV || PLUS_MINUS && T_NUM     = T_NUM
            else if ( (tmp_top == T_NUM || tmp_top == T_INT) && (tmp_pop2 == MUL || tmp_pop2 == DIV || tmp_pop2 == PLUS_MINUS) && (tmp_pop == T_NUM) ){
                if(phCheck(e_stack, h_stack, T_NUM, &tmp_top2, token))
                    return true;
                else{
                    printf("ERROR-12\n");
                    return false;
                }
            }
            // T_INT && PLUS_MINUS || INT || DIV && T_INT    = T_INT;
            else if ( (tmp_top == T_INT) && (tmp_pop2 == INT_DIV || PLUS_MINUS) && (tmp_pop == T_INT) ){
                if(phCheck(e_stack, h_stack, T_INT, &tmp_top2, token))
                    return true;
                else{
                    printf("ERROR-12\n");
                    return false;
                }
            }
            else{
                printf("ERROR9\n");
                return false;
            }
            
        }
        else{
            if(tmp_top == T_DOLLAR && token == T_DOLLAR)
                return true;
            else{
                printf("ERROR8\n");
                return false;
            }
        }
    }
    printf("ERROR-42\n");
    return false;
}

bool pAlgo(stack *e_stack, stack *h_stack, int token){
    int tmp_top = 0;
    stackTop(e_stack,  &tmp_top);

    if(token == T_DOLLAR && tmp_top == T_DOLLAR){
        printf("ERROR61\n");
        return false;
    }

    switch(prec_table[tmp_top][token]){
        case EQ:    // =
            stackPush(e_stack, token);
            return true;
        case LE:    // <
            stackPush(e_stack, LE);
            stackPush(e_stack, token);
            return true;
        case GR:    // >
            if(pHelp(e_stack, h_stack, token))
                return true;
            else{
                printf("ERROR6\n");
                return false;
            }
        case ER:    // else Error
            printf("ERROR5\n");
            return false;
    }
    return false;
}

bool pExpression(){
    bool stringValid = true;

    stack *e_stack = malloc(sizeof(stack));   //TODO: nezabudnúť uvoľniť
    stack *h_stack = malloc(sizeof(stack));     // TODO tento stack preč ak nebude treba
    stackInit(e_stack);
    stackPush(e_stack, T_DOLLAR);

    while(stringValid){
        token *cToken;
        if((cToken = nextToken()) == NULL){
            printf("ERROR4\n");
            return false;   // TODO: Asi nejaké uvolenenie zásobníku ? idk čo všetko sa tu bude robiť ešte a error ?
        }

        if(cToken->type == TOKEN_ID){
            tableItem *item = symGetItem(table, cToken->content.str, scope);
            if(item == NULL){
                printf("ERROR3\n");
                return false;
            }
            if(item->type == FUNC_ID){  //TODO: prerobiť s čekom či je to akčualy error alebo úspešné ukončenie
                free(e_stack);
                free(h_stack);
                
                returnToken = cToken;
                return true;
            }
        }
        if(!pAlgo(e_stack, h_stack, tokConversion(cToken))){
            printf("ERROR1\n");
            return false;
        }

    }
    return true;
}

// --End of file--