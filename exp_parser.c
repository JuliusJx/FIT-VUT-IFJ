/*
 * File: exp_parser.c
 * c file bottom up analysis
 * Subject: IFJ
 * Project: Implementace prekladace imperativniho jazyka IFJ21
 * Authors: Milan Hrabovský (xhrabo15), Ondrej Kovac (xkovac57), Jakub Julius Smykal (xsmyka01), Martin Talajka (xtalaj00)
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
int lastTok = -1;

// Bottom up parser table (extended)
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
    { LE,LE,LE,LE,LE,LE,LE,GR,GR,GR,GR,GR,GR,LE,GR,LE,LE,LE,LE,LE,LE,GR,ER,ER,ER,ER},   //| ..       | 6
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

// Function returns the type of the token for table lookups (retype)
int tokConversion(token *cToken, s_stack *str_stack){
    tableItem *item;
    char temp_str[50] = "";

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
            s_stackPush(str_stack, cToken->content);

            // ### CODE GEN ###
            if(lastTok != TOKEN_ID && lastTok != TOKEN_String && lastTok != TOKEN_Int && lastTok != TOKEN_Num){
                sprintf(temp_str, "\nPUSHS int@%d", atoi(cToken->content));
                if(scope == 1 && !isCondition){ GEN_CODE(&defBuffer, temp_str);}
                else{ GEN_CODE(&blockBuffer, temp_str);}
            }
            return T_INT;

        case TOKEN_Num:
            s_stackPush(str_stack, cToken->content);

            // ### CODE GEN ###
            if(lastTok != TOKEN_ID && lastTok != TOKEN_String && lastTok != TOKEN_Int && lastTok != TOKEN_Num){
                sprintf(temp_str, "\nPUSHS float@%a", atof(cToken->content));
                if(scope == 1 && !isCondition){ GEN_CODE(&defBuffer, temp_str);}
                else{ GEN_CODE(&blockBuffer, temp_str);}
            }
            return T_NUM;

        case TOKEN_String:
            s_stackPush(str_stack, cToken->content);

            // ### CODE GEN ###
            if(lastTok != TOKEN_ID && lastTok != TOKEN_String && lastTok != TOKEN_Int && lastTok != TOKEN_Num){
                sprintf(temp_str, "\nPUSHS string@%s", cToken->content);
                if(scope == 1 && !isCondition){ GEN_CODE(&defBuffer, temp_str);}
                else{ GEN_CODE(&blockBuffer, temp_str);}
            }
            return T_STR;

        case TOKEN_Key_nil:
            return T_NIL;

        case TOKEN_ID:
            item = symGetItem(table, cToken->content, scope);
            sprintf(temp_str, "\nPUSHS TF@");

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
                    s_stackPush(str_stack, item->name);

                    // ### CODE GEN ###
                    if(lastTok != TOKEN_ID && lastTok != TOKEN_String && lastTok != TOKEN_Int && lastTok != TOKEN_Num){
                        if(scope == 1 && !isCondition){
                            GEN_CODE(&defBuffer, temp_str);
                            genVar(&defBuffer, item);
                        }
                        else{
                            GEN_CODE(&blockBuffer, temp_str);
                            genVar(&blockBuffer, item);
                        }
                    }
                    return T_INT_V;
                }
                if(item->type == TYPE_NUM){
                    s_stackPush(str_stack, item->name);

                    // ### CODE GEN ###
                    if(lastTok != TOKEN_ID && lastTok != TOKEN_String && lastTok != TOKEN_Int && lastTok != TOKEN_Num){
                        if(scope == 1 && !isCondition){
                            GEN_CODE(&defBuffer, temp_str);
                            genVar(&defBuffer, item);
                        }
                        else{
                            GEN_CODE(&blockBuffer, temp_str);
                            genVar(&blockBuffer, item);
                        }
                    }
                    return T_NUM_V;
                }
                if(item->type == TYPE_STR){
                    s_stackPush(str_stack, item->name);

                    // ### CODE GEN ###
                    if(lastTok != TOKEN_ID && lastTok != TOKEN_String && lastTok != TOKEN_Int && lastTok != TOKEN_Num){
                        if(scope == 1 && !isCondition){
                            GEN_CODE(&defBuffer, temp_str);
                            genVar(&defBuffer, item);
                        }
                        else{
                            GEN_CODE(&blockBuffer, temp_str);
                            genVar(&blockBuffer, item);
                        }
                    }
                    return T_STR_V;
                }
            }
            else{
                if(item->type == TYPE_INT){
                    s_stackPush(str_stack, item->name);

                    // ### CODE GEN ###
                    if(lastTok != TOKEN_ID && lastTok != TOKEN_String && lastTok != TOKEN_Int && lastTok != TOKEN_Num){
                        if(scope == 1 && !isCondition){
                            GEN_CODE(&defBuffer, temp_str);
                            genVar(&defBuffer, item);
                        }
                        else{
                            GEN_CODE(&blockBuffer, temp_str);
                            genVar(&blockBuffer, item);
                        }
                    }
                    return T_INT_V_NIL;
                }
                if(item->type == TYPE_NUM){
                    s_stackPush(str_stack, item->name);

                    // ### CODE GEN ###
                    if(lastTok != TOKEN_ID && lastTok != TOKEN_String && lastTok != TOKEN_Int && lastTok != TOKEN_Num){
                        if(scope == 1 && !isCondition){
                            GEN_CODE(&defBuffer, temp_str);
                            genVar(&defBuffer, item);
                        }
                        else{
                            GEN_CODE(&blockBuffer, temp_str);
                            genVar(&blockBuffer, item);
                        }
                    }
                    return T_NUM_V_NIL;
                }
                if(item->type == TYPE_STR){
                    s_stackPush(str_stack, item->name);

                    // ### CODE GEN ###
                    if(lastTok != TOKEN_ID && lastTok != TOKEN_String && lastTok != TOKEN_Int && lastTok != TOKEN_Num){
                        if(scope == 1 && !isCondition){
                            GEN_CODE(&defBuffer, temp_str);
                            genVar(&defBuffer, item);
                        }
                        else{
                            GEN_CODE(&blockBuffer, temp_str);
                            genVar(&blockBuffer, item);
                        }
                    }
                    return T_STR_V_NIL;
                }
            }
        default:
            return T_DOLLAR;
    }
}

// Function for convert integer to float in generation
bool pConvertFloat(contentInput* xBuffer, char* temp_str1,  char* temp_str2, int tmp_top, int tmp_pop){
    if( (tmp_top == T_NUM || tmp_top == T_NUM_V) && (tmp_pop == T_INT || tmp_pop == T_INT_V) ){
        GEN_CODE(xBuffer, "\nPOPS ");
        GEN_CODE(xBuffer, temp_str1);    // POPS GF@%%%dtemp1
        GEN_CODE(xBuffer, "\nINT2FLOAT ");
        GEN_CODE(xBuffer, temp_str1);
        GEN_CODE(xBuffer, temp_str1);    // INT2FLOAT GF@%%%dtemp1 GF@%%%dtemp1
        GEN_CODE(xBuffer, "\nPUSHS ");
        GEN_CODE(xBuffer, temp_str1);    // PUSHS GF@%%%dtemp1
    }
    else if( (tmp_top == T_INT || tmp_top == T_INT_V) && (tmp_pop == T_NUM || tmp_pop == T_NUM_V) ){
        GEN_CODE(xBuffer, "\nPOPS ");
        GEN_CODE(xBuffer, temp_str1);    // POPS GF@%%%dtemp1
        GEN_CODE(xBuffer, "\nPOPS ");
        GEN_CODE(xBuffer, temp_str2);    // POPS GF@%%%dtemp2
        GEN_CODE(xBuffer, "\nINT2FLOAT ");
        GEN_CODE(xBuffer, temp_str2);
        GEN_CODE(xBuffer, temp_str2);    // INT2FLOAT GF@%%%dtemp2 GF@%%%dtemp2
        GEN_CODE(xBuffer, "\nPUSHS ");
        GEN_CODE(xBuffer, temp_str2);    // PUSHS GF@%%%dtemp2
        GEN_CODE(xBuffer, "\nPUSHS ");
        GEN_CODE(xBuffer, temp_str1);    // PUSHS GF@%%%dtemp1
    }
    return true;
}

// Function for reducing expression on stack
bool phCheck(stack *e_stack, s_stack *str_stack, int tmp_top, int *tmp_top2, int token){
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

// Function which check rules for expression
bool pHelp(stack *e_stack, s_stack *str_stack, int token){
    int tmp_top = 0;    // Temp variable
    int tmp_top2 = 0;   // Temp variable
    int tmp_pop = 0;    // Temp variable
    int tmp_pop2 = 0;   // Temp variable
    char *str1 = NULL;  // Temp string for code generation
    char *str2 = NULL;  // Temp string for code generation

    stackPop(e_stack, &tmp_pop);
    stackTop(e_stack, &tmp_top);

    // Reduce expression on stack
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
                return false;
            }
        }
    }

    // Try to use a rule to reduce expression
    else{
        stackPop(e_stack, &tmp_pop2);
        stackPop(e_stack, &tmp_top);
        stackTop(e_stack, &tmp_top2);

        // ### CODE GEN SETUP ###
        char temp_str1[20] = "GF@%%%dtemp1 ";
        char temp_str2[20] = "GF@%%%dtemp2 ";
        sprintf(temp_str1, temp_str1, plvl);
        sprintf(temp_str2, temp_str2, plvl);

        if(tmp_top2 == LE){
            stackPop(e_stack, &tmp_top2);

            // T_STR && STR_CONC && T_STR = T_STR
            if((tmp_top == T_STR || tmp_top == T_STR_V) && (tmp_pop2 == STR_CONC) && (tmp_pop == T_STR || tmp_pop == T_STR_V)){
                s_stackPop(str_stack, &str1);
                s_stackPop(str_stack, &str2);
                s_stackPush(str_stack, "STR_CONC");

                // ### CODE GEN ###
                if(scope == 1){
                    GEN_CODE(&defBuffer, "\nPOPS ");
                    GEN_CODE(&defBuffer, temp_str1);    // POPS GF@%%%dtemp1
                    GEN_CODE(&defBuffer, "\nPOPS ");
                    GEN_CODE(&defBuffer, temp_str2);    // POPS GF@%%%dtemp2
                    GEN_CODE(&defBuffer, "\nCONCAT ");
                    GEN_CODE(&defBuffer, temp_str1);
                    GEN_CODE(&defBuffer, temp_str2);
                    GEN_CODE(&defBuffer, temp_str1);    // CONCAT GF@%%%dtemp1 GF@%%%dtemp1 GF@%%%dtemp2
                    GEN_CODE(&defBuffer, "\nPUSHS ");
                    GEN_CODE(&defBuffer, temp_str1);    // PUSHS GF@%%%dtemp1
                }
                else{
                    GEN_CODE(&blockBuffer, "\nPOPS ");
                    GEN_CODE(&blockBuffer, temp_str1);    // POPS GF@%%%dtemp1
                    GEN_CODE(&blockBuffer, "\nPOPS ");
                    GEN_CODE(&blockBuffer, temp_str2);    // POPS GF@%%%dtemp2
                    GEN_CODE(&blockBuffer, "\nCONCAT ");
                    GEN_CODE(&blockBuffer, temp_str1);
                    GEN_CODE(&blockBuffer, temp_str2);
                    GEN_CODE(&blockBuffer, temp_str1);    // CONCAT GF@%%%dtemp1 GF@%%%dtemp1 GF@%%%dtemp2
                    GEN_CODE(&blockBuffer, "\nPUSHS ");
                    GEN_CODE(&blockBuffer, temp_str1);    // PUSHS GF@%%%dtemp1
                }

                // Check if is possible to reduce expression more
                if(phCheck(e_stack, str_stack, T_STR, &tmp_top2, token)){
                    free(str1);
                    free(str2);
                    return true;
                }
                else{
                    free(str1);
                    free(str2);
                    return false;
                }
            }

            // T_INT && INT_DIV || MUL || PLUS || MINUS && T_INT = T_INT
            else if((tmp_top == T_INT || tmp_top == T_INT_V) && (tmp_pop2 == INT_DIV || tmp_pop2 == MUL || tmp_pop2 == PLUS || tmp_pop2 == MINUS) && (tmp_pop == T_INT || tmp_pop == T_INT_V)){
                s_stackPop(str_stack, &str1);
                s_stackPop(str_stack, &str2);

                switch (tmp_pop2){
                    case INT_DIV:
                        s_stackPush(str_stack, "INT_DIV");

                        // ### CODE GEN ###
                        if(scope == 1){ GEN_CODE(&defBuffer, "\nIDIVS"); }
                        else{ GEN_CODE(&blockBuffer, "\nIDIVS"); }
                        break;

                    case MUL:
                        s_stackPush(str_stack, "MUL");

                        // ### CODE GEN ###
                        if(scope == 1){ GEN_CODE(&defBuffer, "\nMULS"); }
                        else{ GEN_CODE(&blockBuffer, "\nMULS"); }
                        break;

                    case PLUS:
                        s_stackPush(str_stack, "PLUS");

                        // ### CODE GEN ###
                        if(scope == 1){ GEN_CODE(&defBuffer, "\nADDS"); }
                        else{ GEN_CODE(&blockBuffer, "\nADDS"); }
                        break;

                    case MINUS:
                        s_stackPush(str_stack, "MINUS");

                        // ### CODE GEN ###
                        if(scope == 1){ GEN_CODE(&defBuffer, "\nSUBS"); }
                        else{ GEN_CODE(&blockBuffer, "\nSUBS"); }
                        break;
                }

                // Check if is possible to reduce expression more
                if(phCheck(e_stack, str_stack, T_INT, &tmp_top2, token)){
                    free(str1);
                    free(str2);
                    return true;
                }
                else{
                    free(str1);
                    free(str2);
                    return false;
                }
            }

            // T_INT && MUL || PLUS || MINUS && T_NUM = T_NUM
            else if((tmp_top == T_INT || tmp_top == T_INT_V) && (tmp_pop2 == MUL || tmp_pop2 == PLUS || tmp_pop2 == MINUS) && (tmp_pop == T_NUM || tmp_pop == T_NUM_V)){
                s_stackPop(str_stack, &str1);
                s_stackPop(str_stack, &str2);

                switch (tmp_pop2){
                    case MUL:
                        s_stackPush(str_stack, "MUL");

                        // ### CODE GEN ###
                        if(scope == 1){
                            pConvertFloat(&defBuffer, temp_str1, temp_str2, tmp_top, tmp_pop);
                            GEN_CODE(&defBuffer, "\nMULS");
                        }
                        else{
                            pConvertFloat(&blockBuffer, temp_str1, temp_str2, tmp_top, tmp_pop);
                            GEN_CODE(&blockBuffer, "\nMULS");
                        }
                        break;

                    case PLUS:
                        s_stackPush(str_stack, "PLUS");

                        // ### CODE GEN ###
                        if(scope == 1){
                            pConvertFloat(&defBuffer, temp_str1, temp_str2, tmp_top, tmp_pop);
                            GEN_CODE(&defBuffer, "\nADDS");
                        }
                        else{
                            pConvertFloat(&blockBuffer, temp_str1, temp_str2, tmp_top, tmp_pop);
                            GEN_CODE(&blockBuffer, "\nADDS");
                        }
                        break;

                    case MINUS:
                        s_stackPush(str_stack, "MINUS");

                        // ### CODE GEN ###
                        if(scope == 1){
                            pConvertFloat(&defBuffer, temp_str1, temp_str2, tmp_top, tmp_pop);
                            GEN_CODE(&defBuffer, "\nSUBS");
                        }
                        else{
                            pConvertFloat(&blockBuffer, temp_str1, temp_str2, tmp_top, tmp_pop);
                            GEN_CODE(&blockBuffer, "\nSUBS");
                        }
                        break;
                }

                // Check if is possible to reduce expression more
                if(phCheck(e_stack, str_stack, T_NUM, &tmp_top2, token)){
                    free(str1);
                    free(str2);
                    return true;
                }
                else{
                    free(str1);
                    free(str2);
                    return false;
                }
            }

            // T_NUM && MUL && T_INT || T_NUM = T_NUM
            else if((tmp_top == T_NUM || tmp_top == T_NUM_V) && (tmp_pop2 == MUL) && (tmp_pop == T_INT || tmp_pop == T_INT_V || tmp_pop == T_NUM || tmp_pop == T_NUM_V)){
                s_stackPop(str_stack, &str1);
                s_stackPop(str_stack, &str2);
                s_stackPush(str_stack, "MUL");

                // ### CODE GEN ###
                if(scope == 1){
                    pConvertFloat(&defBuffer, temp_str1, temp_str2, tmp_top, tmp_pop);
                    GEN_CODE(&defBuffer, "\nMULS");
                }
                else{
                    pConvertFloat(&blockBuffer, temp_str1, temp_str2, tmp_top, tmp_pop);
                    GEN_CODE(&blockBuffer, "\nMULS");
                }

                // Check if is possible to reduce expression more
                if(phCheck(e_stack, str_stack, T_NUM, &tmp_top2, token)){
                    free(str1);
                    free(str2);
                    return true;
                }
                else{
                    free(str1);
                    free(str2);
                    return false;
                }
            }

            // T_INT || T_NUM && DIV && T_INT || T_NUM = T_NUM
            else if((tmp_top == T_INT || tmp_top == T_INT_V || tmp_top == T_NUM || tmp_top == T_NUM_V) && (tmp_pop2 == DIV) && (tmp_pop == T_INT || tmp_pop == T_INT_V || tmp_pop == T_NUM || tmp_pop == T_NUM_V)){
                s_stackPop(str_stack, &str1);
                s_stackPop(str_stack, &str2);
                s_stackPush(str_stack, "DIV");

                // ### CODE GEN ###
                if(scope == 1){
                    pConvertFloat(&defBuffer, temp_str1, temp_str2, tmp_top, tmp_pop);
                    GEN_CODE(&defBuffer, "\nDIVS");
                }
                else{
                    pConvertFloat(&blockBuffer, temp_str1, temp_str2, tmp_top, tmp_pop);
                    GEN_CODE(&blockBuffer, "\nDIVS");
                    }

                // Check if is possible to reduce expression more
                if(phCheck(e_stack, str_stack, T_NUM, &tmp_top2, token)){
                    free(str1);
                    free(str2);
                    return true;
                }
                else{
                    free(str1);
                    free(str2);
                    return false;
                }
            }

            // T_NUM && PLUS || MINUS && T_NUM || T_INT = T_NUM
            else if((tmp_top == T_NUM || tmp_top == T_NUM_V) && (tmp_pop2 == PLUS || tmp_pop2 == MINUS) && (tmp_pop == T_NUM || tmp_pop == T_NUM_V || tmp_pop == T_INT || tmp_pop == T_INT_V)){
                s_stackPop(str_stack, &str1);
                s_stackPop(str_stack, &str2);

                switch (tmp_pop2){
                    case PLUS:
                        s_stackPush(str_stack, "PLUS");

                        // ### CODE GEN ###
                        if(scope == 1){
                            pConvertFloat(&defBuffer, temp_str1, temp_str2, tmp_top, tmp_pop);
                            GEN_CODE(&defBuffer, "\nADDS");
                        }
                        else{
                            pConvertFloat(&blockBuffer, temp_str1, temp_str2, tmp_top, tmp_pop);
                            GEN_CODE(&blockBuffer, "\nADDS");
                            }
                        break;

                    case MINUS:
                        s_stackPush(str_stack, "MINUS");

                        // ### CODE GEN ###
                        if(scope == 1){
                            pConvertFloat(&defBuffer, temp_str1, temp_str2, tmp_top, tmp_pop);
                            GEN_CODE(&defBuffer, "\nSUBS");
                        }
                        else{
                            pConvertFloat(&blockBuffer, temp_str1, temp_str2, tmp_top, tmp_pop);
                            GEN_CODE(&blockBuffer, "\nSUBS");
                        }
                        break;
                }

                // Check if is possible to reduce expression more
                if(phCheck(e_stack, str_stack, T_NUM, &tmp_top2, token)){
                    free(str1);
                    free(str2);
                    return true;
                }
                else{
                    free(str1);
                    free(str2);
                    return false;
                }
            }

            // T_NUM || T_INT && == || != || < || > || <= || >= && T_NUM || T_INT = T_BOOL
            else if((tmp_top == T_NUM || tmp_top == T_NUM_V || tmp_top == T_INT || tmp_top == T_INT_V) && \
                    (tmp_pop2 == R_EQ || tmp_pop2 == R_NEQ || tmp_pop2 == R_LE || tmp_pop2 == R_GR || tmp_pop2 == R_LEQ || tmp_pop2 == R_GRQ) && \
                    (tmp_pop == T_NUM || tmp_pop == T_NUM_V || tmp_pop == T_INT || tmp_pop == T_INT_V)){
                s_stackPop(str_stack, &str1);
                s_stackPop(str_stack, &str2);

                switch (tmp_pop2){
                    case R_EQ:
                        s_stackPush(str_stack, "R_EQ");

                        // ### CODE GEN ###
                        pConvertFloat(&blockBuffer, temp_str1,  temp_str2, tmp_top, tmp_pop);
                        GEN_CODE(&blockBuffer, "\nEQS");
                        break;

                    case R_NEQ:
                        s_stackPush(str_stack, "R_NEQ");

                        // ### CODE GEN ###
                        pConvertFloat(&blockBuffer, temp_str1,  temp_str2, tmp_top, tmp_pop);
                        GEN_CODE(&blockBuffer, "\nEQS\nNOTS");
                        break;

                    case R_LE:
                        s_stackPush(str_stack, "R_LE");

                        // ### CODE GEN ###
                        pConvertFloat(&blockBuffer, temp_str1,  temp_str2, tmp_top, tmp_pop);
                        GEN_CODE(&blockBuffer, "\nLTS");
                        break;

                    case R_GR:
                        s_stackPush(str_stack, "R_GR");

                        // ### CODE GEN ###
                        pConvertFloat(&blockBuffer, temp_str1,  temp_str2, tmp_top, tmp_pop);
                        GEN_CODE(&blockBuffer, "\nGTS");
                        break;

                    case R_LEQ:
                    case R_GRQ:
                        if(tmp_pop2 == R_LEQ)
                            s_stackPush(str_stack, "R_LEQ");
                        else
                            s_stackPush(str_stack, "R_GRQ");

                        // ### CODE GEN ###
                        pConvertFloat(&blockBuffer, temp_str1,  temp_str2, tmp_top, tmp_pop);

                        GEN_CODE(&blockBuffer, "\nPOPS ");
                        GEN_CODE(&blockBuffer, temp_str1);    // POPS GF@%%%dtemp1
                        GEN_CODE(&blockBuffer, "\nPOPS ");
                        GEN_CODE(&blockBuffer, temp_str2);    // POPS GF@%%%dtemp2
                        GEN_CODE(&blockBuffer, "\nPUSHS ")
                        GEN_CODE(&blockBuffer, temp_str2);    // PUSHS GF@%%%dtemp1
                        GEN_CODE(&blockBuffer, "\nPUSHS ")
                        GEN_CODE(&blockBuffer, temp_str1);    // PUSHS GF@%%%dtemp2
                        if(tmp_pop2 == R_LEQ){ GEN_CODE(&blockBuffer, "\nLTS"); }
                        else{ GEN_CODE(&blockBuffer, "\nGTS"); }
                        GEN_CODE(&blockBuffer, "\nPUSHS ")
                        GEN_CODE(&blockBuffer, temp_str2);    // PUSHS GF@%%%dtemp1
                        GEN_CODE(&blockBuffer, "\nPUSHS ")
                        GEN_CODE(&blockBuffer, temp_str1);    // PUSHS GF@%%%dtemp2
                        GEN_CODE(&blockBuffer, "\nEQS")
                        GEN_CODE(&blockBuffer, "\nORS");
                        break;
                }

                // Check if is possible to reduce expression more
                if(phCheck(e_stack, str_stack, T_BOOL, &tmp_top2, token)){
                    free(str1);
                    free(str2);
                    return true;
                }
                else{
                    free(str1);
                    free(str2);
                    return false;
                }
            }

            // T_STR && == || != || < || > || <= || >= && T_STR = T_BOOL
            else if((tmp_top == T_STR || tmp_top == T_STR_V) && (tmp_pop2 == R_EQ || tmp_pop2 == R_NEQ || tmp_pop2 == R_LE || tmp_pop2 == R_GR || tmp_pop2 == R_LEQ || tmp_pop2 == R_GRQ) && (tmp_pop == T_STR || tmp_pop == T_STR_V)){
                s_stackPop(str_stack, &str1);
                s_stackPop(str_stack, &str2);

                switch (tmp_pop2){
                    case R_EQ:
                        s_stackPush(str_stack, "R_EQ");

                        // ### CODE GEN ###
                        GEN_CODE(&blockBuffer, "\nEQS");
                        break;

                    case R_NEQ:

                        // ### CODE GEN ###
                        GEN_CODE(&blockBuffer, "\nEQS\nNOTS");
                        s_stackPush(str_stack, "R_NEQ");
                        break;

                    case R_LE:

                        // ### CODE GEN ###
                        GEN_CODE(&blockBuffer, "\nLTS");
                        s_stackPush(str_stack, "R_LE");
                        break;

                    case R_GR:

                        // ### CODE GEN ###
                        GEN_CODE(&blockBuffer, "\nGTS");
                        s_stackPush(str_stack, "R_GR");
                        break;

                    case R_LEQ:
                    case R_GRQ:
                        if(tmp_pop2 == R_LEQ)
                            s_stackPush(str_stack, "R_LEQ");
                        else
                            s_stackPush(str_stack, "R_GRQ");

                        GEN_CODE(&blockBuffer, "\nPOPS ");
                        GEN_CODE(&blockBuffer, temp_str1);    // POPS GF@%%%dtemp1
                        GEN_CODE(&blockBuffer, "\nPOPS ");
                        GEN_CODE(&blockBuffer, temp_str2);    // POPS GF@%%%dtemp2
                        GEN_CODE(&blockBuffer, "\nPUSHS ")
                        GEN_CODE(&blockBuffer, temp_str2);    // PUSHS GF@%%%dtemp1
                        GEN_CODE(&blockBuffer, "\nPUSHS ")
                        GEN_CODE(&blockBuffer, temp_str1);    // PUSHS GF@%%%dtemp2
                        if(tmp_pop2 == R_LEQ){ GEN_CODE(&blockBuffer, "\nLTS"); }
                        else{ GEN_CODE(&blockBuffer, "\nGTS"); }
                        GEN_CODE(&blockBuffer, "\nPUSHS ")
                        GEN_CODE(&blockBuffer, temp_str2);    // PUSHS GF@%%%dtemp1
                        GEN_CODE(&blockBuffer, "\nPUSHS ")
                        GEN_CODE(&blockBuffer, temp_str1);    // PUSHS GF@%%%dtemp2
                        GEN_CODE(&blockBuffer, "\nEQS")
                        GEN_CODE(&blockBuffer, "\nORS");
                        break;
                }

                // Check if is possible to reduce expression more
                if(phCheck(e_stack, str_stack, T_BOOL, &tmp_top2, token)){
                    free(str1);
                    free(str2);
                    return true;
                }
                else{
                    free(str1);
                    free(str2);
                    return false;
                }
            }

            // NIL RULES
            // T_INT_V || T_INT_V_NIL || T_NUM_V || T_NUM_V_NIL || T_STR_V || T_STR_V_NIL && == || != && T_NIL = T_BOOL
            else if( (tmp_top == T_INT_V || tmp_top == T_INT_V_NIL || tmp_top == T_NUM_V || tmp_top == T_NUM_V_NIL || tmp_top == T_STR_V || tmp_top == T_STR_V_NIL) && (tmp_pop2 == R_EQ || tmp_pop2 == R_NEQ) && tmp_pop == T_NIL){
                s_stackPop(str_stack, &str1);
                s_stackPop(str_stack, &str2);

                switch (tmp_pop2){
                    case R_EQ:
                        s_stackPush(str_stack, "R_EQ");

                        // ### CODE GEN ###
                        GEN_CODE(&blockBuffer, "\nPUSHS nil@nil\nEQS");
                        break;
                    case R_NEQ:
                        s_stackPush(str_stack, "R_NEQ");

                        // ### CODE GEN ###
                        GEN_CODE(&blockBuffer, "\nPUSHS nil@nil\nEQS\nNOTS");
                        break;  
                }

                // Check if is possible to reduce expression more
                if(phCheck(e_stack, str_stack, T_BOOL, &tmp_top2, token)){
                    free(str1);
                    free(str2);
                    return true;
                }
                else{
                    free(str1);
                    free(str2);
                    return false;
                }
            }

            // T_NIL && == || != && T_INT_V || T_INT_V_NIL || T_NUM_V || T_NUM_V_NIL || T_STR_V || T_STR_V_NIL = T_BOOL
            else if(tmp_top == T_NIL && (tmp_pop2 == R_EQ || tmp_pop2 == R_NEQ) && (tmp_pop == T_INT_V || tmp_pop == T_INT_V_NIL || tmp_pop == T_NUM_V || tmp_pop == T_NUM_V_NIL || tmp_pop == T_STR_V || tmp_pop == T_STR_V_NIL)){
                s_stackPop(str_stack, &str1);
                s_stackPop(str_stack, &str2);

                switch (tmp_pop2){
                    case R_EQ:
                        s_stackPush(str_stack, "R_EQ");

                        // ### CODE GEN ###
                        GEN_CODE(&blockBuffer, "\nPUSHS nil@nil\nEQS");
                        break;
                    case R_NEQ:
                        s_stackPush(str_stack, "R_NEQ");

                        // ### CODE GEN ###
                        GEN_CODE(&blockBuffer, "\nPUSHS nil@nil\nEQS\nNOTS");
                        break; 
                }

                // Check if is possible to reduce expression more
                if(phCheck(e_stack, str_stack, T_BOOL, &tmp_top2, token)){
                    free(str1);
                    free(str2);
                    return true;
                }
                else{
                    free(str1);
                    free(str2);
                    return false;
                }
            }

            // T_INT_V || T_INT_V_NIL || T_NUM_V || T_NUM_V_NIL && == | != && T_INT_V || T_INT_V_NIL || T_NUM_V || T_NUM_V_NIL = T_BOOL
            else if((tmp_top == T_INT_V || tmp_top == T_INT_V_NIL || tmp_top == T_NUM_V || tmp_top == T_NUM_V_NIL) && (tmp_pop2 == R_EQ || tmp_pop2 == R_NEQ) && (tmp_pop == T_INT_V || tmp_pop == T_INT_V_NIL || tmp_pop == T_NUM_V || tmp_pop == T_NUM_V_NIL)){
                s_stackPop(str_stack, &str1);
                s_stackPop(str_stack, &str2);

                switch (tmp_pop2){
                    case R_EQ:
                        s_stackPush(str_stack, "R_EQ");

                        // ### CODE GEN ###
                        GEN_CODE(&blockBuffer, "\nEQS");
                        break;
                    case R_NEQ:
                        s_stackPush(str_stack, "R_NEQ");

                        // ### CODE GEN ###
                        GEN_CODE(&blockBuffer, "\nEQS\nNOTS");
                        break;
                }

                // Check if is possible to reduce expression more
                if(phCheck(e_stack, str_stack, T_BOOL, &tmp_top2, token)){
                    free(str1);
                    free(str2);
                    return true;
                }
                else{
                    free(str1);
                    free(str2);
                    return false;
                }
            }

            // T_STR_V || T_STR_V_NIL && == || != && T_STR_V || T_STR_V_NIL = T_BOOL
            else if((tmp_top == T_STR_V || tmp_top == T_STR_V_NIL) && (tmp_pop2 == R_EQ || tmp_pop2 == R_NEQ) && (tmp_pop == T_STR_V || tmp_pop == T_STR_V_NIL)){
                s_stackPop(str_stack, &str1);
                s_stackPop(str_stack, &str2);

                switch (tmp_pop2){
                    case R_EQ:
                        s_stackPush(str_stack, "R_EQ");

                        // ### CODE GEN ###
                        GEN_CODE(&blockBuffer, "\nEQS");
                        break;
                    case R_NEQ:
                        s_stackPush(str_stack, "R_NEQ");

                        // ### CODE GEN ###
                        GEN_CODE(&blockBuffer, "\nEQS\nNOTS");
                        break;
                }

                // Check if is possible to reduce expression more
                if(phCheck(e_stack, str_stack, T_BOOL, &tmp_top2, token)){
                    free(str1);
                    free(str2);
                    return true;
                }
                else{
                    free(str1);
                    free(str2);
                    return false;
                }
            }

            // T_NIL && == || != && T_NIL = T_BOOL
            else if(tmp_top == T_NIL && (tmp_pop2 == R_EQ || tmp_pop2 == R_NEQ) && tmp_pop == T_NIL){
                s_stackPop(str_stack, &str1);
                s_stackPop(str_stack, &str2);

                switch (tmp_pop2){
                    case R_EQ:
                        s_stackPush(str_stack, "R_EQ");

                        // ### CODE GEN ###
                        GEN_CODE(&blockBuffer, "\nPUSHS nil@nil\nPUSHS nil@nil\nEQS");
                        break;
                    case R_NEQ:
                        s_stackPush(str_stack, "R_NEQ");

                        // ### CODE GEN ###
                        GEN_CODE(&blockBuffer, "\nPUSHS nil@nil\nPUSHS nil@nil\nEQS\nNOTS");
                        break;
                }

                // Check if is possible to reduce expression more
                if(phCheck(e_stack, str_stack, T_BOOL, &tmp_top2, token)){
                    free(str1);
                    free(str2);
                    return true;
                }
                else{
                    free(str1);
                    free(str2);
                    return false;
                }
            }

            // If no rule is found
            else{
                if(errCode == 0)
                    errCode = 6;
                return false;
            }
        }

        // Special cases
        else{
            // STRLEN
            if(tmp_pop2 == STR_LEN && tmp_top == LE){
                // # STR
                if((tmp_pop2 == STR_LEN) && (tmp_pop == T_STR || tmp_pop == T_STR_V)){
                    s_stackPop(str_stack, &str1);
                    s_stackPush(str_stack, "STR_LEN");

                    if(scope == 1){
                        GEN_CODE(&defBuffer, "\nPOPS ");
                        GEN_CODE(&defBuffer, temp_str1);    // POPS GF@%%%dtemp1
                        GEN_CODE(&defBuffer, "\nSTRLEN ");
                        GEN_CODE(&defBuffer, temp_str1);
                        GEN_CODE(&defBuffer, temp_str1);    // STRLEN GF@%%%dtemp1
                        GEN_CODE(&defBuffer, "\nPUSHS ");
                        GEN_CODE(&defBuffer, temp_str1);    // PUSH GF@%%%dtemp1
                    }
                    else{
                        GEN_CODE(&blockBuffer, "\nPOPS ");
                        GEN_CODE(&blockBuffer, temp_str1);    // POPS GF@%%%dtemp1
                        GEN_CODE(&blockBuffer, "\nSTRLEN ");
                        GEN_CODE(&blockBuffer, temp_str1);
                        GEN_CODE(&blockBuffer, temp_str1);    // STRLEN GF@%%%dtemp1
                        GEN_CODE(&blockBuffer, "\nPUSHS ");
                        GEN_CODE(&blockBuffer, temp_str1);    // PUSH GF@%%%dtemp1
                    }

                    // Check if is possible to reduce expression more
                    if(phCheck(e_stack, str_stack, T_INT, &tmp_top2, token)){
                        free(str1);
                        return true;
                    }
                    else{
                        free(str1);
                        return false;
                    }
                }
                else{
                    if(errCode == 0)
                        errCode = 6;
                    return false;
                }
            }

            // Left and right par
            else if(tmp_pop2 == LEFT_PAR && tmp_top == LE){
                if(token == RIGHT_PAR){
                    stackPush(e_stack, tmp_top);
                    stackPush(e_stack, tmp_pop);
                    return true;
                }
                else{
                    if(errCode == 0)
                        errCode = 2;
                    return false;
                }
            }

            // Expression is reduced and no next token is found
            else if(tmp_top == T_DOLLAR && token == T_DOLLAR){
                stackPush(e_stack, tmp_top);
                stackPush(e_stack, tmp_pop);
                return true;
            }
            else{
                if(errCode == 0)
                    errCode = 2;
                return false;
            }
        }
    }
    return false;
}

// Algorithm for Bottom-up parsing
// Returns true if successful otherwise false
bool pAlgo(stack *e_stack, s_stack *str_stack, int token){
    char *rm = NULL;
    int tmp_top = 0;
    stackTop(e_stack,  &tmp_top);

    if(tmp_top == T_BOOL)
        tmp_top = T_INT;

    if(token == T_DOLLAR && tmp_top == T_DOLLAR){
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
                return false;
            }
        case ER:    // Error
            s_stackPop(str_stack, &rm);
            free(rm);

            if(token == T_NIL || token == T_INT_V_NIL || token == T_NUM_V_NIL || token == T_STR_V_NIL){
                errCode = 8;
            }
            return false;
    }
    return false;
}

// Function frees stacks and it's content
void cClean(stack *e_stack, s_stack *str_stack){
    int e_dump;
    char *s_dump;;

    while(!stackIsEmpty(e_stack))
        stackPop(e_stack, &e_dump);
    while(!s_stackIsEmpty(str_stack)){
        s_stackPop(str_stack, &s_dump);
        free(s_dump);
    }

    free(e_stack);
    free(str_stack);
}

// Function checks if expression is valid 
// Returns true if expression is valid otherwise false if expression is not valid
bool pExpression(int lvl){
    char *str_stack_top = NULL; // Top of string stack
    int value = -1;             // Temp value for checking if expression returns correct type (returned value)
    tableItem *sym_value;       // Temp value for checking if expression returns correct type (expected value)
    stringValid = true;         // Flag for checking if expression is still valid after every token
    cToken = NULL;              // Current token
    lastTok = -1;               // Last token type
    plvl = lvl;                 // Current level of recursion (comma)

    // Initialize stacks
    s_stack *str_stack = malloc(sizeof(stack));     // TODO: error 99
    stack *e_stack = malloc(sizeof(stack));         // TODO: error 99
    s_stackInit(str_stack);
    stackInit(e_stack);
    stackPush(e_stack, T_DOLLAR);

    // ### CODE GEN ###
    // Generate temp variable for storing result of expression
    if(lvl > gTmp){
        char temp_str[50] = "";
        sprintf(temp_str, "\nDEFVAR GF@%%%dtemp1\nDEFVAR GF@%%%dtemp2", plvl,plvl);
        GEN_CODE(&startBuffer, temp_str);
        gTmp += 1;
    }

    // Main loop for token loading and expression parsing
    while(stringValid){
        if(cToken != NULL)
            freeToken(cToken);
        if((cToken = nextToken()) == NULL){
            cClean(e_stack, str_stack);
            return false;
        }

        // Check if we can process token
        if((cToken->type >= TOKEN_Int && cToken->type <= TOKEN_LessEQ) || cToken->type == TOKEN_EQ || cToken->type == TOKEN_NotEQ || cToken->type == TOKEN_LeftPar || cToken->type == TOKEN_RightPar \
            || cToken->type == TOKEN_String || cToken->type == TOKEN_ID || cToken->type == TOKEN_Key_nil || cToken->type == TOKEN_StrLen){

            // Process token
            if(!pAlgo(e_stack, str_stack, tokConversion(cToken, str_stack))){
                stringValid = false;
                break;
            }
        }
        // Check if already loaded expression is valid
        else{
            if(stackIsEmpty(e_stack)){
                cClean(e_stack, str_stack);
                return false;
            }
            else{
                if(pAlgo(e_stack, str_stack, tokConversion(cToken, str_stack))){
                    stackPop(e_stack, &value);
                    stringValid = false;
                }
                else{
                    cClean(e_stack, str_stack);
                    return false;
                }
            }
        }
        // Save last token type
        if(cToken != NULL)
            lastTok = cToken->type;
    }

    // Check if token is comma (more return values)
    // If so, check if expression is valid
    // If not, return token
    if(cToken->type == TOKEN_Comma){
        if(!pExpression(lvl+1)){
            cClean(e_stack, str_stack);
            return false;
        }
    }
    else
        returnToken = cToken;

    // Check if expression returns correct type
    // Parser expects return value
    if(!symstackIsEmpty(symStack)){
        int stack_top = 0;

        // If e_stack is not empty, get expression value
        if(!stackIsEmpty(e_stack)){
            stackTop(e_stack, &stack_top);
            if(stack_top != T_DOLLAR){
                if(pAlgo(e_stack, str_stack, T_DOLLAR)){
                    stackPop(e_stack, &value);
                }
            }
        }

        // Get expected return type
        symstackPop(symStack, &sym_value);

        // Check if type is bool and generate true/false instead of 1/0
        if(!s_stackIsEmpty(str_stack) && sym_value->type == T_BOOL){
            char temp_str[50] = "";

            s_stackTop(str_stack, &str_stack_top);
            if(!strcmp(str_stack_top, "1")){

                value = T_BOOL;
                sprintf(temp_str, "GF@%%%dtemp1", plvl);

                // ### CODE GEN ###
                GEN_CODE(&blockBuffer, "\nPOPS ");
                GEN_CODE(&blockBuffer, temp_str);
                GEN_CODE(&blockBuffer, "\nPUSHS bool@true");
            }
            else if(!strcmp(str_stack_top, "0")){
                value = T_BOOL;
                sprintf(temp_str, "GF@%%%dtemp1", plvl);

                // ### CODE GEN ###
                GEN_CODE(&blockBuffer, "\nPOPS ");
                GEN_CODE(&blockBuffer, temp_str);
                GEN_CODE(&blockBuffer, "\nPUSHS bool@false");
            }
        }

        // Retype value to type value for compatibility
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
            break;
        default:
            break;
        }

        // Check if returned type is correct or is it float and integer/float
        if(value == sym_value->type || (sym_value->type == TYPE_NUM && (value == TYPE_INT || value == TYPE_NUM) )){
            if(!sym_value->isInit)
                sym_value->isInit = true;

            // ### CODE GEN ###
            if(scope == 1 && value != T_BOOL){
                if(sym_value->scope == 0){ GEN_CODE(&defBuffer, "\nPOPS LF@"); }
                else{ GEN_CODE(&defBuffer, "\nPOPS TF@"); }
                genVar(&defBuffer, sym_value);
            }
            else if(value == T_BOOL){
                if(sym_value->scope == 0){ GEN_CODE(&blockBuffer, "\nPOPS LF@"); }
                else{ GEN_CODE(&blockBuffer, "\nPOPS TF@"); }
                genVar(&blockBuffer, sym_value);
                isCondition = false;
            }
            else{
                if(sym_value->scope == 0){ GEN_CODE(&blockBuffer, "\nPOPS LF@"); }
                else{ GEN_CODE(&blockBuffer, "\nPOPS TF@"); }
                genVar(&blockBuffer, sym_value);
            }
            errCode = 0;
            cClean(e_stack, str_stack);
            return true;
        }

        else if(value == T_NIL && sym_value->type == T_BOOL){
            // ### CODE GEN ###
            if(sym_value->scope == 0){ GEN_CODE(&blockBuffer, "\nPOPS LF@"); }
            else{ GEN_CODE(&blockBuffer, "\nPOPS TF@"); }
            genVar(&blockBuffer, sym_value);
            errCode = 0;
            cClean(e_stack, str_stack);
            return true;
        }

        else if(value == T_NIL){
            sym_value->isInit = false;
            errCode = 0;
            cClean(e_stack, str_stack);
            return true;
        }

        // Error, return type is not correct - type incompatibility
        else if(value == TYPE_NUM && sym_value->type == TYPE_INT){
            errCode = 4;
            cClean(e_stack, str_stack);
            return false;
        }

        // If value is integer, float or string and expected type is bool
        else if((value == TYPE_INT || value == TYPE_NUM || value == TYPE_STR) && sym_value->type == T_BOOL){
            char temp_str[50] = "";
            sprintf(temp_str, "GF@%%%dtemp1 ", plvl);

            // ### CODE GEN ###
            if(value == TYPE_STR){                      // Get string length
                GEN_CODE(&blockBuffer, "\nPOPS ");
                GEN_CODE(&blockBuffer, temp_str);
                GEN_CODE(&blockBuffer, "\nSTRLEN ");
                GEN_CODE(&blockBuffer, temp_str);
                GEN_CODE(&blockBuffer, temp_str);       // STRLEN GF@%%%dtemp1 GF@%%%dtemp1
                GEN_CODE(&blockBuffer, "\nINT2FLOAT ");
                GEN_CODE(&blockBuffer, temp_str);
                GEN_CODE(&blockBuffer, temp_str);       // INT2FLOAT GF@%%%dtemp1 GF@%%%dtemp1
                GEN_CODE(&blockBuffer, "\nPUSHS ");
                GEN_CODE(&blockBuffer, temp_str);       // PUSHS GF@%%%dtemp1
            }
            else if(value == TYPE_INT){                 // Convert integer to float
                GEN_CODE(&blockBuffer, "\nPOPS ");
                GEN_CODE(&blockBuffer, temp_str);
                GEN_CODE(&blockBuffer, "\nINT2FLOAT ");
                GEN_CODE(&blockBuffer, temp_str);
                GEN_CODE(&blockBuffer, temp_str);       // INT2FLOAT GF@%%%dtemp1 GF@%%%dtemp1
                GEN_CODE(&blockBuffer, "\nPUSHS ");
                GEN_CODE(&blockBuffer, temp_str);       // PUSHS GF@%%%dtemp1
            }

            GEN_CODE(&blockBuffer, "\nPUSHS float@0x0p+0");
            GEN_CODE(&blockBuffer, "\nEQS");
            GEN_CODE(&blockBuffer, "\nNOTS");

            if(sym_value->scope == 0){ GEN_CODE(&blockBuffer, "\nPOPS LF@"); }
            else{ GEN_CODE(&blockBuffer, "\nPOPS TF@"); }
            genVar(&blockBuffer, sym_value);
            errCode = 0;
            cClean(e_stack, str_stack);
            return true;
        }
        // Return type is not correct
        else{
            if(errCode == 0)
                errCode = 4;
            cClean(e_stack, str_stack);
            return false;
        }
    }
    // Parser did not expected return value
    else{
        // Expression is valid
        if(errCode == 0){
            cClean(e_stack, str_stack);
            return true;
        }
        // Expression is not valid
        else{
            cClean(e_stack, str_stack);
            return false;
        }
    }
}

// --End of file--