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

token *cToken;
bool stringValid = true;

                // TOP          // VSTUP
int prec_table[PREC_TAB_SIZE][PREC_TAB_SIZE] = {
//  | 0  | 1  | 2  | 3 | 4   | 5  |  6   |   7  | 8 | 9 | 10     | 11      | 12    | 13   | 14   | 15   | 16   | 17    | 18
//  | #  | *  | /  |// | +,- | .. | Rel1 | Rel2 | ( | ) | i(int) | i(num)  |i(str) |i(i-v)|i(n-v)|i(s-v)| $    | Nil   | Nil-val
    { EQ,  GR,  GR, GR,  GR,   GR,  GR,    GR,   LE, GR,  GR,      GR,      GR,       GR,   GR,   GR,    GR,    ER,     ER},    //| #      | 0
    { LE,  GR,  GR, GR,  GR,   GR,  GR,    GR,   LE, GR,  LE,      LE,      LE,       LE,   LE,   LE,    GR,    ER,     ER},    //| *      | 1
    { LE,  GR,  GR, GR,  GR,   GR,  GR,    GR,   LE, GR,  LE,      LE,      LE,       LE,   LE,   LE,    GR,    ER,     ER},    //| /      | 2
    { LE,  GR,  GR, GR,  GR,   GR,  GR,    GR,   LE, GR,  LE,      LE,      LE,       LE,   LE,   LE,    GR,    ER,     ER},    //| //     | 3
    { LE,  LE,  LE, LE,  GR,   GR,  GR,    GR,   LE, GR,  LE,      LE,      LE,       LE,   LE,   LE,    GR,    ER,     ER},    //| +, -   | 4
    { LE,  LE,  LE, LE,  LE,   LE,  GR,    GR,   LE, GR,  LE,      LE,      LE,       LE,   LE,   LE,    GR,    ER,     ER},    //| ..     | 5
    { LE,  LE,  LE, LE,  LE,   GR,  ER,    ER,   LE, GR,  LE,      LE,      LE,       LE,   LE,   LE,    GR,    LE,     LE},    //| Rel1   | 6
    //########################################################################################################################
    { LE,  LE,  LE, LE,  LE,   GR,  ER,    ER,   LE, GR,  LE,      LE,      LE,       LE,   LE,   LE,    GR,    ER,     ER},    //| Rel2   | 7
    //------------------------------------------------------------------------------------------------------------------------
    { LE,  LE,  LE, LE,  LE,   LE,  LE,    LE,   LE, EQ,  LE,      LE,      LE,       LE,   LE,   LE,    ER,    LE,     LE},    //| (      | 8
    { GR,  GR,  GR, GR,  GR,   GR,  GR,    GR,   ER, GR,  ER,      ER,      ER,       ER,   ER,   ER,    GR,    LE,     LE},    //| )      | 9
    { GR,  GR,  GR, GR,  GR,   GR,  GR,    GR,   ER, GR,  ER,      ER,      ER,       ER,   ER,   ER,    GR,    ER,     ER},    //| i(int) | 10
    { GR,  GR,  GR, GR,  GR,   GR,  GR,    GR,   ER, GR,  ER,      ER,      ER,       ER,   ER,   ER,    GR,    ER,     ER},    //| i(num) | 11
    { GR,  GR,  GR, GR,  GR,   GR,  GR,    GR,   ER, GR,  ER,      ER,      ER,       ER,   ER,   ER,    GR,    ER,     ER},    //| i(str) | 12
    //########################################################################################################################
    { GR,  GR,  GR, GR,  GR,   GR,  GR,    GR,   ER, GR,  ER,      ER,      ER,       ER,   ER,   ER,    GR,    ER,     ER},    //| i(i-v) | 13
    { GR,  GR,  GR, GR,  GR,   GR,  GR,    GR,   ER, GR,  ER,      ER,      ER,       ER,   ER,   ER,    GR,    ER,     ER},    //| i(n-v) | 14
    { GR,  GR,  GR, GR,  GR,   GR,  GR,    GR,   ER, GR,  ER,      ER,      ER,       ER,   ER,   ER,    GR,    ER,     ER},    //| i(s-v) | 15
    //------------------------------------------------------------------------------------------------------------------------
    { LE,  LE,  LE, LE,  LE,   LE,  LE,    LE,   LE, ER,  LE,      LE,      LE,       LE,   LE,   LE,    ER,    LE,     LE},    //| $      | 16
    { ER,  ER,  ER, ER,  ER,   ER,  GR,    ER,   ER, GR,  ER,      ER,      ER,       ER,   ER,   ER,    GR,    ER,     ER},    //| Nil    | 17
    { ER,  ER,  ER, ER,  ER,   ER,  GR,    ER,   ER, GR,  ER,      ER,      ER,       ER,   ER,   ER,    GR,    ER,     ER}     //| Nil-val| 18
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
            return REL_COMP_2;
        case TOKEN_EQ:
        case TOKEN_NotEQ:
            return REL_COMP_1;
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
        case TOKEN_Key_nil:
            return T_NIL;
        case TOKEN_ID:
            item = symGetItem(table, cToken->content.str, scope);
            if(item == NULL)
                return T_DOLLAR;
            if(item->type == FUNC_ID){
                stringValid = false;
                return T_DOLLAR;
            }
            if(item->type == TYPE_NIL)
                return T_NIL;

            if(item->isInit){
                if(item->type == TYPE_INT)
                    return T_INT_V;
                if(item->type == TYPE_NUM)
                    return T_NUM_V;
                if(item->type == TYPE_STR)
                    return T_STR_V;
            }
            else
                return T_VAL_NIL;
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
            // T_STR && STR_CONC && T_STR = T_STR
            if( (tmp_top == T_STR || tmp_top == T_STR_V) && (tmp_pop2 == STR_CONC) && (tmp_pop == T_STR || tmp_pop == T_STR_V) ){
                if(phCheck(e_stack, h_stack, T_STR, &tmp_top2, token))
                    return true;
                else{
                    printf("ERROR-12\n");
                    return false;
                }
            }
            // T_INT && INT_DIV || MUL || PLUS_MINUS && T_INT      = T_INT
            else if( (tmp_top == T_INT || tmp_top == T_INT_V) && (tmp_pop2 == INT_DIV || tmp_pop2 == MUL || tmp_pop2 == PLUS_MINUS) && (tmp_pop == T_INT || tmp_pop == T_INT_V) ){
                if(phCheck(e_stack, h_stack, T_INT, &tmp_top2, token))
                    return true;
                else{
                    printf("ERROR-12\n");
                    return false;
                }
            }
            // T_INT && MUL || PLUS_MINUS && T_NUM = T_NUM
            else if( (tmp_top == T_INT || tmp_top == T_INT_V) && (tmp_pop2 == MUL || tmp_pop2 == PLUS_MINUS) && (tmp_pop == T_NUM || tmp_pop == T_NUM_V) ){
                if(phCheck(e_stack, h_stack, T_NUM, &tmp_top2, token))
                    return true;
                else{
                    printf("ERROR-12\n");
                    return false;
                }
            }
            // T_NUM && MUL && T_INT || T_NUM     = T_NUM
            else if( (tmp_top == T_NUM || tmp_top == T_NUM_V) && (tmp_pop2 == MUL) && (tmp_pop == T_INT || tmp_pop == T_INT_V || tmp_pop == T_NUM || tmp_pop == T_NUM_V) ){
                if(phCheck(e_stack, h_stack, T_NUM, &tmp_top2, token))
                    return true;
                else{
                    printf("ERROR-12\n");
                    return false;
                }
            }
            // T_INT || T_NUM && DIV && T_INT || T_NUM = T_NUM
            else if( (tmp_top == T_INT || tmp_top == T_INT_V || tmp_top == T_NUM || tmp_top == T_NUM_V) && (tmp_pop2 == DIV) && (tmp_pop == T_INT || tmp_pop == T_INT_V) ){
                if(phCheck(e_stack, h_stack, T_NUM, &tmp_top2, token))
                    return true;
                else{
                    printf("ERROR-12\n");
                    return false;
                }
            }
            // T_NUM && PLUS_MINUS && T_NUM || T_INT = T_NUM
            else if( (tmp_top == T_NUM || tmp_top == T_NUM_V) && (tmp_pop2 == PLUS_MINUS) && (tmp_pop == T_NUM || tmp_pop == T_NUM_V || tmp_pop == T_INT || tmp_pop == T_INT_V) ){
                if(phCheck(e_stack, h_stack, T_NUM, &tmp_top2, token))
                    return true;
                else{
                    printf("ERROR-12\n");
                    return false;
                }
            }
            // T_NUM || T_INT &&  REL_COMP && T_NUM || T_INT = T_BOOL
            else if( (tmp_top == T_NUM || tmp_top == T_NUM_V || tmp_top == T_INT || tmp_top == T_INT_V) && (tmp_pop2 == REL_COMP_1 || tmp_pop2 == REL_COMP_2) && (tmp_pop == T_NUM || tmp_pop == T_NUM_V || tmp_pop == T_INT || tmp_pop == T_INT_V) ){
                if(phCheck(e_stack, h_stack, T_BOOL, &tmp_top2, token))
                    return true;
                else{
                    printf("ERROR-12\n");
                    return false;
                }
            }
            // T_STR && REL_COMP && T_STR = T_BOOL
            else if( (tmp_top == T_STR || tmp_top == T_STR_V) && (tmp_pop2 == REL_COMP_1 || tmp_pop2 == REL_COMP_2) && (tmp_pop == T_STR || tmp_pop == T_STR_V) ){
                if(phCheck(e_stack, h_stack, T_BOOL, &tmp_top2, token))
                    return true;
                else{
                    printf("ERROR-12\n");
                    return false;
                }
            }

            // Variable && ==, ~= && nil -> T_BOOL  //TODO:
            else if( (tmp_top == T_INT_V || tmp_top == T_NUM_V || tmp_top == T_STR_V || tmp_top == T_NIL) && tmp_pop2 == REL_COMP_1 && tmp_pop == T_NIL){
                if(phCheck(e_stack, h_stack, T_BOOL, &tmp_top2, token))
                    return true;
                else{
                    printf("ERROR-12\n");
                    return false;
                }
            }

            else{
                printf("ERROR9\n");
                errCode = 6;
                return false;
            }
            
        }
        else{
            if(tmp_pop2 == STR_LEN && tmp_top == LE){
                if((tmp_pop2 == STR_LEN) && (tmp_pop == T_STR || tmp_pop == T_STR_V))
                    if(phCheck(e_stack, h_stack, T_INT, &tmp_top2, token))
                        return true;
                    else{
                        printf("ERROR-364");
                        return false;
                    }
                else{
                    printf("ERROR-12\n");
                    errCode = 6;
                    return false;
                }
            }
            else if(tmp_pop2 == LEFT_PAR && tmp_top == LE){
                if(token == RIGHT_PAR){
                    stackPush(e_stack, tmp_top);
                    stackPush(e_stack, tmp_pop);
                    return true;
                }
                else{
                    printf("ERROR4587\n");
                    errCode = 2;
                    return false;
                }
            }

            else if(tmp_top == T_DOLLAR && token == T_DOLLAR){
                stackPush(e_stack, tmp_top);
                stackPush(e_stack, tmp_pop);
                return true;
                }
            else{
                printf("ERROR8\n");
                errCode = 2;
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

    if(tmp_top == T_BOOL)
        tmp_top = T_INT;

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
    printf("ERROR-456\n");
    return false;
}

bool pExpression(){
    stringValid = true;
    int value = 0;
    tableItem *sym_value;

    stack *e_stack = malloc(sizeof(stack));   //TODO: nezabudnúť uvoľniť
    stack *h_stack = malloc(sizeof(stack));     // TODO tento stack preč ak nebude treba
    stackInit(e_stack);
    stackPush(e_stack, T_DOLLAR);

    while(stringValid){
        if((cToken = nextToken()) == NULL){
            printf("ERROR4\n");
            return false;   // TODO: Asi nejaké uvolenenie zásobníku ? idk čo všetko sa tu bude robiť ešte a error ?
        }

        if((cToken->type >= TOKEN_Int && cToken->type <= TOKEN_LessEQ) || cToken->type == TOKEN_EQ || cToken->type == TOKEN_NotEQ || cToken->type == TOKEN_LeftPar || cToken->type == TOKEN_RightPar || cToken->type == TOKEN_String || cToken->type == TOKEN_ID || cToken->type == TOKEN_Key_nil){
            //spracujem     TOKEN_Comma 34  - , cToken->type == TOKEN_Comma
            if(!pAlgo(e_stack, h_stack, tokConversion(cToken))){
                stringValid = false;
                break;
            }
        }
        else{
            if(stackIsEmpty(e_stack)){
                printf("ERROR77\n");
                return false;
            }
            else{
                if(pAlgo(e_stack, h_stack, tokConversion(cToken))){
                    stackPop(e_stack, &value);
                    stringValid = false;
                }
                else{
                    printf("ERROR66\n");
                    return false;
                }
            }
        }
    }

    if(cToken->type == TOKEN_Comma)
        pExpression();
    else
        returnToken = cToken;

    if(!symstackIsEmpty(symStack)){
        int stack_top = 0;

        if(!stackIsEmpty(e_stack)){
            stackTop(e_stack, &stack_top);
            if(stack_top != T_DOLLAR){
                if(pAlgo(e_stack, h_stack, T_DOLLAR)){
                    stackPop(e_stack, &value);
                }
                else{
                    printf("ERROR4444\n");
                }
            }
        }
        else{
            printf("ERROR456546\n");
        }

        symstackPop(symStack, &sym_value);
        switch (value){
        case T_INT:
        case T_INT_V:
            value = TYPE_INT;
            break;
        case T_NUM:
        case T_NUM_V:
            value = TYPE_NUM;
            break;
        case T_STR:
        case T_STR_V:
            value = TYPE_STR;
            break;
        default:
            printf("BOIII ERROR\n");
            break;
        }

        if(value == (sym_value->type)){
            return true;
        }
        else if(value == T_NIL){
            sym_value->isInit = false;
            return true;
        }
        else{
            printf("ERROR-987\n");
            errCode = 4;
            return false;
        }
    }
    else
        return true;
}

// --End of file--