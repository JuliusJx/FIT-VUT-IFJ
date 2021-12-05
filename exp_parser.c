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
int gTmp = -1;
int plvl = 0;

                // TOP          // VSTUP
int prec_table[PREC_TAB_SIZE][PREC_TAB_SIZE] = {
//                                                |int     |int-var       |int-var-nil
//                                                |  |num  |  |num-var    |  |num-var-nil
//                                                |  |  |str  |  |str-var |  |  |str-var-nil
//   |0 |1 |2 |3 |4 |5 |6 |7 |8 |9 |10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|
//   |# |* |/ |//|+ |- |..|==|~=|< |> |<=|>=|( |) |i |i |i |iv|iv|iv|$|Nil|in|in|in|
    { EQ,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,LE,GR,GR,GR,GR,GR,GR,GR,GR,ER,ER,ER,ER},   //| #        | 0
    { LE,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,LE,GR,LE,LE,LE,LE,LE,LE,GR,ER,ER,ER,ER},   //| *        | 1
    { LE,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,LE,GR,LE,LE,LE,LE,LE,LE,GR,ER,ER,ER,ER},   //| /        | 2
    { LE,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,LE,GR,LE,LE,LE,LE,LE,LE,GR,ER,ER,ER,ER},   //| //       | 3
    { LE,LE,LE,LE,GR,GR,GR,GR,GR,GR,GR,GR,GR,LE,GR,LE,LE,LE,LE,LE,LE,GR,ER,ER,ER,ER},   //| +        | 4
    { LE,LE,LE,LE,GR,GR,GR,GR,GR,GR,GR,GR,GR,LE,GR,LE,LE,LE,LE,LE,LE,GR,ER,ER,ER,ER},   //| -        | 5
    { LE,LE,LE,LE,LE,LE,GR,GR,GR,GR,GR,GR,GR,LE,GR,LE,LE,LE,LE,LE,LE,GR,ER,ER,ER,ER},   //| ..       | 6
    { LE,LE,LE,LE,LE,LE,GR,ER,ER,ER,ER,ER,ER,LE,GR,LE,LE,LE,LE,LE,LE,GR,LE,LE,LE,LE},   //| ==       | 7
    { LE,LE,LE,LE,LE,LE,GR,ER,ER,ER,ER,ER,ER,LE,GR,LE,LE,LE,LE,LE,LE,GR,LE,LE,LE,LE},   //| ~=       | 8
    { LE,LE,LE,LE,LE,LE,GR,ER,ER,ER,ER,ER,ER,LE,GR,LE,LE,LE,LE,LE,LE,GR,ER,ER,ER,ER},   //| <        | 9
    { LE,LE,LE,LE,LE,LE,GR,ER,ER,ER,ER,ER,ER,LE,GR,LE,LE,LE,LE,LE,LE,GR,ER,ER,ER,ER},   //| >        | 10
    { LE,LE,LE,LE,LE,LE,GR,ER,ER,ER,ER,ER,ER,LE,GR,LE,LE,LE,LE,LE,LE,GR,ER,ER,ER,ER},   //| <=       | 11
    { LE,LE,LE,LE,LE,LE,GR,ER,ER,ER,ER,ER,ER,LE,GR,LE,LE,LE,LE,LE,LE,GR,ER,ER,ER,ER},   //| >=       | 12
    { LE,LE,LE,LE,LE,LE,LE,LE,LE,LE,LE,LE,LE,LE,EQ,LE,LE,LE,LE,LE,LE,ER,LE,LE,LE,LE},   //| (        | 13
    { GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,ER,GR,ER,ER,ER,ER,ER,ER,GR,LE,LE,LE,LE},   //| )        | 14
    { GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,ER,GR,ER,ER,ER,ER,ER,ER,GR,ER,ER,ER,ER},   //| i(int)   | 15
    { GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,ER,GR,ER,ER,ER,ER,ER,ER,GR,ER,ER,ER,ER},   //| i(num)   | 16
    { GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,ER,GR,ER,ER,ER,ER,ER,ER,GR,ER,ER,ER,ER},   //| i(str)   | 17
    { GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,ER,GR,ER,ER,ER,ER,ER,ER,GR,ER,ER,ER,ER},   //| i(i-v)   | 18
    { GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,ER,GR,ER,ER,ER,ER,ER,ER,GR,ER,ER,ER,ER},   //| i(n-v)   | 19
    { GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,GR,ER,GR,ER,ER,ER,ER,ER,ER,GR,ER,ER,ER,ER},   //| i(s-v)   | 20
    { LE,LE,LE,LE,LE,LE,LE,LE,LE,LE,LE,LE,LE,LE,ER,LE,LE,LE,LE,LE,LE,ER,LE,LE,LE,LE},   //| $        | 21
    { ER,ER,ER,ER,ER,ER,ER,GR,GR,ER,ER,ER,ER,ER,GR,ER,ER,ER,ER,ER,ER,GR,ER,ER,ER,ER},   //| Nil      | 22
    { ER,ER,ER,ER,ER,ER,ER,GR,GR,ER,ER,ER,ER,ER,GR,ER,ER,ER,ER,ER,ER,GR,ER,ER,ER,ER},   //| i-int-nil| 23
    { ER,ER,ER,ER,ER,ER,ER,GR,GR,ER,ER,ER,ER,ER,GR,ER,ER,ER,ER,ER,ER,GR,ER,ER,ER,ER},   //| i-num-nil| 24
    { ER,ER,ER,ER,ER,ER,ER,GR,GR,ER,ER,ER,ER,ER,GR,ER,ER,ER,ER,ER,ER,GR,ER,ER,ER,ER}    //| i-str-nil| 25
};

int tokConversion(token *cToken, s_stack *str_stack){
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
            return PLUS;
        case TOKEN_Minus:
            return MINUS;
        case TOKEN_Concat:
            return STR_CONC;
        case TOKEN_Greater:
            return R_GR;
        case TOKEN_GreatEQ:
            return R_GRQ;
        case TOKEN_Less:
            return R_LE;
        case TOKEN_LessEQ:
            return R_LEQ;
        case TOKEN_EQ:
            return R_EQ;
        case TOKEN_NotEQ:
            return R_NEQ;
        case TOKEN_LeftPar:
            return LEFT_PAR;
        case TOKEN_RightPar:
            return RIGHT_PAR;
        case TOKEN_Int:
            // ### CODE GEN ###
            printf("PUSHS int@%d\n", atoi(cToken->content));
            s_stackPush(str_stack, cToken->content);
            return T_INT;
        case TOKEN_Num:
            // ### CODE GEN ###
            printf("PUSHS float@%a\n", atof(cToken->content));
            s_stackPush(str_stack, cToken->content);
            return T_NUM;
        case TOKEN_String:
            // ### CODE GEN ###
            printf("PUSHS string@%s\n", cToken->content);
            s_stackPush(str_stack, cToken->content);
            return T_STR;
        case TOKEN_Key_nil:
            return T_NIL;
        case TOKEN_ID:
            item = symGetItem(table, cToken->content, scope);
            if(item == NULL)
                return T_DOLLAR;
            if(item->type == FUNC_ID){
                stringValid = false;
                return T_DOLLAR;
            }
            if(item->type == TYPE_NIL)
                return T_NIL;

            if(item->isInit){
                if(item->type == TYPE_INT){
                    // ### CODE GEN ###
                    printf("PUSHS TF@%s\n", item->name);
                    s_stackPush(str_stack, item->name);
                    return T_INT_V;
                }
                if(item->type == TYPE_NUM){
                    // ### CODE GEN ###
                    printf("PUSHS TF@%s\n", item->name);
                    s_stackPush(str_stack, item->name);
                    return T_NUM_V;
                }
                if(item->type == TYPE_STR){
                    // ### CODE GEN ###
                    printf("PUSHS TF@%s\n", item->name);
                    s_stackPush(str_stack, item->name);
                    return T_STR_V;
                }
            }
            else{
                if(item->type == TYPE_INT){
                    // ### CODE GEN ###
                    printf("PUSHS TF@%s\n", item->name);
                    s_stackPush(str_stack, item->name);
                    return T_INT_V_NIL;
                }
                if(item->type == TYPE_NUM){
                    // ### CODE GEN ###
                    printf("PUSHS TF@%s\n", item->name);
                    s_stackPush(str_stack, item->name);
                    return T_NUM_V_NIL;
                }
                if(item->type == TYPE_STR){
                    // ### CODE GEN ###
                    printf("PUSHS TF@%s\n", item->name);
                    s_stackPush(str_stack, item->name);
                    return T_STR_V_NIL;
                }
            }
        default:
            return T_DOLLAR;
    }
}

bool phCheck(stack *e_stack, s_stack *str_stack, int tmp_top, int *tmp_top2, int token){        // TODO: prepísať tie tmp_topy na niečo rozumnejšie
    stackTop(e_stack, tmp_top2);
    if(prec_table[*tmp_top2][token] == LE){
        stackPush(e_stack, LE);
        stackPush(e_stack, tmp_top);
        stackPush(e_stack, token);
        return true;
    }
    else{
        stackPush(e_stack, tmp_top);
        if(pHelp(e_stack, str_stack, token))
            return true;
        else
            return false;
    }
}

bool pHelp(stack *e_stack, s_stack *str_stack, int token){
    int tmp_top = 0;
    int tmp_top2 = 0;
    int tmp_pop = 0;
    int tmp_pop2 = 0;
    char *str1 = NULL;
    char *str2 = NULL;
    char *str_top = NULL;

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
            if(pHelp(e_stack, str_stack, token))
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
                s_stackPop(str_stack, &str1);
                s_stackPop(str_stack, &str2);
                s_stackPush(str_stack, "STR_CONC");
                s_stackTop(str_stack, &str_top);
                printf("[%s %s %s]\n", str2, str_top, str1);

                //### CODE GEN ###
                printf("POPS TF@%%%dtemp1\n",plvl);
                printf("POPS TF@%%%dtemp2\n",plvl);
                printf("CONCAT TF@%%%dtemp1 TF@%%%dtemp1 TF@%%%dtemp2\n", plvl, plvl, plvl);
                printf("PUSHS TF@%%%dtemp1\n", plvl);

                if(phCheck(e_stack, str_stack, T_STR, &tmp_top2, token))
                    return true;
                else{
                    printf("ERROR-12\n");
                    return false;
                }
            }
            // T_INT && INT_DIV || MUL || PLUS_MINUS && T_INT      = T_INT
            else if( (tmp_top == T_INT || tmp_top == T_INT_V) && (tmp_pop2 == INT_DIV || tmp_pop2 == MUL || tmp_pop2 == PLUS || tmp_pop2 == MINUS) && (tmp_pop == T_INT || tmp_pop == T_INT_V) ){
                s_stackPop(str_stack, &str1);
                s_stackPop(str_stack, &str2);
                switch (tmp_pop2){
                case INT_DIV:
                    s_stackPush(str_stack, "INT_DIV");
                    break;
                case MUL:
                    s_stackPush(str_stack, "MUL");
                    break;
                case PLUS:
                    s_stackPush(str_stack, "PLUS");
                    break;
                case MINUS:
                    s_stackPush(str_stack, "MINUS");
                    break;
                }
                s_stackTop(str_stack, &str_top);
                printf("[%s %s %s]\n", str2, str_top, str1);

                if(phCheck(e_stack, str_stack, T_INT, &tmp_top2, token))
                    return true;
                else{
                    printf("ERROR-12\n");
                    return false;
                }
            }
            // T_INT && MUL || PLUS_MINUS && T_NUM = T_NUM
            else if( (tmp_top == T_INT || tmp_top == T_INT_V) && (tmp_pop2 == MUL || tmp_pop2 == PLUS || tmp_pop2 == MINUS) && (tmp_pop == T_NUM || tmp_pop == T_NUM_V) ){
                s_stackPop(str_stack, &str1);
                s_stackPop(str_stack, &str2);
                switch (tmp_pop2){
                case MUL:
                    s_stackPush(str_stack, "MUL");
                    break;
                case PLUS:
                    s_stackPush(str_stack, "PLUS");
                    break;
                case MINUS:
                    s_stackPush(str_stack, "MINUS");
                    break;
                }
                s_stackTop(str_stack, &str_top);
                printf("[%s %s %s]\n", str2, str_top, str1);
                
                if(phCheck(e_stack, str_stack, T_NUM, &tmp_top2, token))
                    return true;
                else{
                    printf("ERROR-12\n");
                    return false;
                }
            }
            // T_NUM && MUL && T_INT || T_NUM     = T_NUM
            else if( (tmp_top == T_NUM || tmp_top == T_NUM_V) && (tmp_pop2 == MUL) && (tmp_pop == T_INT || tmp_pop == T_INT_V || tmp_pop == T_NUM || tmp_pop == T_NUM_V) ){
                s_stackPop(str_stack, &str1);
                s_stackPop(str_stack, &str2);
                s_stackPush(str_stack, "MUL");
                s_stackTop(str_stack, &str_top);
                printf("[%s %s %s]\n", str2, str_top, str1);

                // ### CODE GEN ###

                if(phCheck(e_stack, str_stack, T_NUM, &tmp_top2, token))
                    return true;
                else{
                    printf("ERROR-12\n");
                    return false;
                }
            }
            // T_INT || T_NUM && DIV && T_INT || T_NUM = T_NUM
            else if( (tmp_top == T_INT || tmp_top == T_INT_V || tmp_top == T_NUM || tmp_top == T_NUM_V) && (tmp_pop2 == DIV) && (tmp_pop == T_INT || tmp_pop == T_INT_V || tmp_pop == T_NUM || tmp_pop == T_NUM_V) ){
                s_stackPop(str_stack, &str1);
                s_stackPop(str_stack, &str2);
                s_stackPush(str_stack, "DIV");
                s_stackTop(str_stack, &str_top);
                printf("[%s %s %s]\n", str2, str_top, str1);

                if(phCheck(e_stack, str_stack, T_NUM, &tmp_top2, token))
                    return true;
                else{
                    printf("ERROR-12\n");
                    return false;
                }
            }
            // T_NUM && PLUS_MINUS && T_NUM || T_INT = T_NUM
            else if( (tmp_top == T_NUM || tmp_top == T_NUM_V) && (tmp_pop2 == PLUS || tmp_pop2 == MINUS) && (tmp_pop == T_NUM || tmp_pop == T_NUM_V || tmp_pop == T_INT || tmp_pop == T_INT_V) ){
                s_stackPop(str_stack, &str1);
                s_stackPop(str_stack, &str2);
                switch (tmp_pop2){
                case PLUS:
                    s_stackPush(str_stack, "PLUS");
                    break;
                case MINUS:
                    s_stackPush(str_stack, "MINUS");
                    break;
                }
                s_stackTop(str_stack, &str_top);
                printf("[%s %s %s]\n", str2, str_top, str1);

                if(phCheck(e_stack, str_stack, T_NUM, &tmp_top2, token))
                    return true;
                else{
                    printf("ERROR-12\n");
                    return false;
                }
            }
            // T_NUM || T_INT &&  REL_COMP && T_NUM || T_INT = T_BOOL
            else if( (tmp_top == T_NUM || tmp_top == T_NUM_V || tmp_top == T_INT || tmp_top == T_INT_V) && \
                     (tmp_pop2 == R_EQ || tmp_pop2 == R_NEQ || tmp_pop2 == R_LE || tmp_pop2 == R_GR || tmp_pop2 == R_LEQ || tmp_pop2 == R_GRQ) && \
                     (tmp_pop == T_NUM || tmp_pop == T_NUM_V || tmp_pop == T_INT || tmp_pop == T_INT_V) ){
                s_stackPop(str_stack, &str1);
                s_stackPop(str_stack, &str2);
                switch (tmp_pop2){
                case R_EQ:
                    s_stackPush(str_stack, "R_EQ");
                    break;
                case R_NEQ:
                    s_stackPush(str_stack, "R_NEQ");
                    break;
                case R_LE:
                    s_stackPush(str_stack, "R_LE");
                    break;
                case R_GR:
                    s_stackPush(str_stack, "R_GR");
                    break;
                case R_LEQ:
                    s_stackPush(str_stack, "R_LEQ");
                    break;
                case R_GRQ:
                    s_stackPush(str_stack, "R_GRQ");
                    break;    
                }
                s_stackTop(str_stack, &str_top);
                printf("[%s %s %s]\n", str2, str_top, str1);

                if(phCheck(e_stack, str_stack, T_BOOL, &tmp_top2, token))
                    return true;
                else{
                    printf("ERROR-12\n");
                    return false;
                }
            }
            // T_STR && REL_COMP && T_STR = T_BOOL
            else if( (tmp_top == T_STR || tmp_top == T_STR_V) && (tmp_pop2 == R_EQ || tmp_pop2 == R_NEQ || tmp_pop2 == R_LE || tmp_pop2 == R_GR || tmp_pop2 == R_LEQ || tmp_pop2 == R_GRQ) && (tmp_pop == T_STR || tmp_pop == T_STR_V) ){
                s_stackPop(str_stack, &str1);
                s_stackPop(str_stack, &str2);
                switch (tmp_pop2){
                case R_EQ:
                    s_stackPush(str_stack, "R_EQ");
                    break;
                case R_NEQ:
                    s_stackPush(str_stack, "R_NEQ");
                    break;
                case R_LE:
                    s_stackPush(str_stack, "R_LE");
                    break;
                case R_GR:
                    s_stackPush(str_stack, "R_GR");
                    break;
                case R_LEQ:
                    s_stackPush(str_stack, "R_LEQ");
                    break;
                case R_GRQ:
                    s_stackPush(str_stack, "R_GRQ");
                    break;    
                }
                s_stackTop(str_stack, &str_top);
                printf("[%s %s %s]\n", str2, str_top, str1);

                if(phCheck(e_stack, str_stack, T_BOOL, &tmp_top2, token))
                    return true;
                else{
                    printf("ERROR-12\n");
                    return false;
                }
            }

            // NIL RULES
            // T_INT_V || T_INT_V_NIL || T_NUM_V || T_NUM_V_NIL || T_STR_V || T_STR_V_NIL && REL_COMP_1 && T_NIL = T_BOOL
            else if( (tmp_top == T_INT_V || tmp_top == T_INT_V_NIL || tmp_top == T_NUM_V || tmp_top == T_NUM_V_NIL || tmp_top == T_STR_V || tmp_top == T_STR_V_NIL) && (tmp_pop2 == R_EQ || tmp_pop2 == R_NEQ) && tmp_pop == T_NIL){
                s_stackPop(str_stack, &str1);
                s_stackPop(str_stack, &str2);
                switch (tmp_pop2){
                case R_EQ:
                    s_stackPush(str_stack, "R_EQ");
                    break;
                case R_NEQ:
                    s_stackPush(str_stack, "R_NEQ");
                    break;  
                }
                s_stackTop(str_stack, &str_top);
                printf("[%s %s %s]\n", str2, str_top, str1);

                if(phCheck(e_stack, str_stack, T_BOOL, &tmp_top2, token))
                    return true;
                else{
                    printf("ERROR-12\n");
                    return false;
                }
            }
            // T_NIL && REL_COMP_1 && T_INT_V || T_INT_V_NIL || T_NUM_V || T_NUM_V_NIL || T_STR_V || T_STR_V_NIL = T_BOOL   // ? TODO: týmto som si nie úplne istý
            else if(tmp_top == T_NIL && (tmp_pop2 == R_EQ || tmp_pop2 == R_NEQ) && (tmp_pop == T_INT_V || tmp_pop == T_INT_V_NIL || tmp_pop == T_NUM_V || tmp_pop == T_NUM_V_NIL || tmp_pop == T_STR_V || tmp_pop == T_STR_V_NIL)){
                s_stackPop(str_stack, &str1);
                s_stackPop(str_stack, &str2);
                switch (tmp_pop2){
                case R_EQ:
                    s_stackPush(str_stack, "R_EQ");
                    break;
                case R_NEQ:
                    s_stackPush(str_stack, "R_NEQ");
                    break; 
                }
                s_stackTop(str_stack, &str_top);
                printf("[%s %s %s]\n", str2, str_top, str1);

                if(phCheck(e_stack, str_stack, T_BOOL, &tmp_top2, token))
                    return true;
                else{
                    printf("ERROR-12\n");
                    return false;
                }
            }
            // T_INT_V || T_INT_V_NIL || T_NUM_V || T_NUM_V_NIL   &&   REL_COMP_1   &&   T_INT_V || T_INT_V_NIL || T_NUM_V || T_NUM_V_NIL = T_BOOL
            else if( (tmp_top == T_INT_V || tmp_top == T_INT_V_NIL || tmp_top == T_NUM_V || tmp_top == T_NUM_V_NIL) && (tmp_pop2 == R_EQ || tmp_pop2 == R_NEQ) && (tmp_pop == T_INT_V || tmp_pop == T_INT_V_NIL || tmp_pop == T_NUM_V || tmp_pop == T_NUM_V_NIL) ){
                s_stackPop(str_stack, &str1);
                s_stackPop(str_stack, &str2);
                switch (tmp_pop2){
                case R_EQ:
                    s_stackPush(str_stack, "R_EQ");
                    break;
                case R_NEQ:
                    s_stackPush(str_stack, "R_NEQ");
                    break;
                }
                s_stackTop(str_stack, &str_top);
                printf("[%s %s %s]\n", str2, str_top, str1);

                if(phCheck(e_stack, str_stack, T_BOOL, &tmp_top2, token))
                    return true;
                else{
                    printf("ERROR-12\n");
                    return false;
                }
            }
            // T_STR_V || T_STR_V_NIL   &&   REL_COMP_1   &&   T_STR_V || T_STR_V_NIL = T_BOOL
            else if( (tmp_top == T_STR_V || tmp_top == T_STR_V_NIL) && (tmp_pop2 == R_EQ || tmp_pop2 == R_NEQ) && (tmp_pop == T_STR_V || tmp_pop == T_STR_V_NIL) ){
                s_stackPop(str_stack, &str1);
                s_stackPop(str_stack, &str2);
                switch (tmp_pop2){
                case R_EQ:
                    s_stackPush(str_stack, "R_EQ");
                    break;
                case R_NEQ:
                    s_stackPush(str_stack, "R_NEQ");
                    break;
                }
                s_stackTop(str_stack, &str_top);
                printf("[%s %s %s]\n", str2, str_top, str1);

                if(phCheck(e_stack, str_stack, T_BOOL, &tmp_top2, token))
                    return true;
                else{
                    printf("ERROR-12\n");
                    return false;
                }
            }
            // T_NIL   &&   REL_COMP_1   &&   T_NIL = T_BOOL
            else if(tmp_top == T_NIL && (tmp_pop2 == R_EQ || tmp_pop2 == R_NEQ) && tmp_pop == T_NIL){
                s_stackPop(str_stack, &str1);
                s_stackPop(str_stack, &str2);
                switch (tmp_pop2){
                case R_EQ:
                    s_stackPush(str_stack, "R_EQ");
                    break;
                case R_NEQ:
                    s_stackPush(str_stack, "R_NEQ");
                    break;
                }
                s_stackTop(str_stack, &str_top);
                printf("[%s %s %s]\n", str2, str_top, str1);

                if(phCheck(e_stack, str_stack, T_BOOL, &tmp_top2, token))
                    return true;
                else{
                    printf("ERROR-12\n");
                    return false;
                }
            }

            else{
                printf("ERROR9\n");
                if(errCode == 0)
                    errCode = 6;
                return false;
            }

        }
        else{
            if(tmp_pop2 == STR_LEN && tmp_top == LE){
                if((tmp_pop2 == STR_LEN) && (tmp_pop == T_STR || tmp_pop == T_STR_V)){
                    s_stackPop(str_stack, &str1);
                    s_stackPush(str_stack, "STR_LEN");
                    s_stackTop(str_stack, &str_top);
                    printf("[%s %s]\n", str1, str_top);


                    if(phCheck(e_stack, str_stack, T_INT, &tmp_top2, token))
                        return true;
                    else{
                        printf("ERROR-364");
                        return false;
                    }
                }
                else{
                    printf("ERROR-12\n");
                    if(errCode == 0)
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
                    if(errCode == 0)
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
                if(errCode == 0)
                    errCode = 2;
                return false;
            }
        }
    }
    printf("ERROR-42\n");
    return false;
}

bool pAlgo(stack *e_stack, s_stack *str_stack, int token){
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
            if(pHelp(e_stack, str_stack, token))
                return true;
            else{
                printf("ERROR6\n");
                return false;
            }
        case ER:    // else Error
            printf("ERROR5\n");
            if(token == T_NIL || token == T_INT_V_NIL || token == T_NUM_V_NIL || token == T_STR_V_NIL){
                errCode = 8;
                return false;
            }
            return false;
    }
    printf("ERROR-456\n");
    return false;
}

bool pExpression(int lvl){
    stringValid = true;
    int value = -1;
    tableItem *sym_value;
    plvl = lvl;

    stack *e_stack = malloc(sizeof(stack));   //TODO: nezabudnúť uvoľniť
    s_stack *str_stack = malloc(sizeof(stack));
    stackInit(e_stack);
    stackPush(e_stack, T_DOLLAR);
    s_stackInit(str_stack);

    // ### CODE GEN ###
    if(lvl > gTmp){
        printf("DEFVAR TF@%%%dtemp1\nDEFVAR TF@%%%dtemp2\n",plvl,plvl);
        gTmp += 1;
    }

    while(stringValid){
        if((cToken = nextToken()) == NULL){
            printf("ERROR4\n");
            return false;   // TODO: Asi nejaké uvolenenie zásobníku ? idk čo všetko sa tu bude robiť ešte a error ?
        }

        if((cToken->type >= TOKEN_Int && cToken->type <= TOKEN_LessEQ) || cToken->type == TOKEN_EQ || cToken->type == TOKEN_NotEQ || cToken->type == TOKEN_LeftPar || cToken->type == TOKEN_RightPar \
            || cToken->type == TOKEN_String || cToken->type == TOKEN_ID || cToken->type == TOKEN_Key_nil || cToken->type == TOKEN_StrLen){
            //spracujem     TOKEN_Comma 34  - , cToken->type == TOKEN_Comma
            if(!pAlgo(e_stack, str_stack, tokConversion(cToken, str_stack))){
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
                if(pAlgo(e_stack, str_stack, tokConversion(cToken, str_stack))){
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
        pExpression(lvl+1);
    else
        returnToken = cToken;

    if(!symstackIsEmpty(symStack)){
        int stack_top = 0;

        if(!stackIsEmpty(e_stack)){
            stackTop(e_stack, &stack_top);
            if(stack_top != T_DOLLAR){
                if(pAlgo(e_stack, str_stack, T_DOLLAR)){
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
        case T_NIL:
            break;
        case T_BOOL:
            value = TYPE_INT;
            break;
        default:
            printf("BOIII ERROR\n");
            break;
        }

        if(value == sym_value->type || (sym_value->type == TYPE_NUM && (value == TYPE_INT || value == TYPE_NUM) )){
            if(!sym_value->isInit)
                sym_value->isInit = true;

            // ### CODE GEN ###
            printf("POPS TF@[X]\n");

            return true;
        }
        else if(value == T_NIL){
            sym_value->isInit = false;
            return true;
        }
        else if(value == TYPE_NUM && sym_value->type == TYPE_INT){
            printf("ERROR-987\n");
            errCode = 4;
            return false;
        }
        else{
            return false;
        }
    }
    else{
        if(errCode == 0)
            return true;
        else
            return false;
    }
}

// --End of file--