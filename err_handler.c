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
#include "generator.h"
#include "scanner.h"

#define RECEIVED \
        fprintf(stderr, "Instead received \"");\
        printToken(token);\
        fprintf(stderr, "\"");


int errCode = 0;


void errPrint( int errType, token *token, char *err_string){
    errCode = errType;
    




    fprintf(stderr, "ERROR: Line: %d Type: ", token->line);

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

     fprintf(stderr, "\n");

    if (!strcmp(err_string, "bad_token")){
        fprintf(stderr, "Token \"");
        printToken(token);
        token = GetToken();
        printToken(token);
        fprintf(stderr, "\" could not be identiied! ");
    }else if (!strcmp(err_string, "exp_require")){
        fprintf(stderr, "Missing keyword \"require\" needed at the beginning of program! ");
        RECEIVED
    }
    else if (!strcmp(err_string, "exp_ifj21")){
        fprintf(stderr, "Unknown header \"ifj21\"! ");
        RECEIVED
    }
    else if (!strcmp(err_string, "exp_string")){
        fprintf(stderr, "Expected string literal. ");
        RECEIVED
    }
    else if (!strcmp(err_string, "exp_eof")){
        fprintf(stderr, "Expected end of input file (EOF). ");
        RECEIVED
    }
    else if (!strcmp(err_string, "exp_id")){
        fprintf(stderr, "Expected ID. ");
        RECEIVED
    }
    else if (!strcmp(err_string, "id_defined")){
        fprintf(stderr, "ID \"");
        printToken(token);
        fprintf(stderr, "\" is already defined elsewhere in this scope.");
    }
    else if (!strcmp(err_string, "exp_colon")){
        fprintf(stderr, "Expected \":\". ");
        RECEIVED
    }
    else if (!strcmp(err_string, "exp_function")){
        fprintf(stderr, "Expected \"function\". ");
        RECEIVED
    }
    else if (!strcmp(err_string, "exp_leftpar")){
        fprintf(stderr, "Expected \"(\". ");
        RECEIVED
    }
    else if (!strcmp(err_string, "exp_rightpar")){
        fprintf(stderr, "Expected \")\". ");
        RECEIVED
    }
    else if (!strcmp(err_string, "func_init")){
        fprintf(stderr, "Tried to define an already defined function \"");
        printToken(token);
        fprintf(stderr, "\".");
    }
    else if (!strcmp(err_string, "no_rets_in_def")){
        fprintf(stderr, "Expected return types (missing colon?) ");
        RECEIVED
    }
    else if (!strcmp(err_string, "exp_end")){
        fprintf(stderr, "Expected \"end\". ");
        RECEIVED
    }
    else if (!strcmp(err_string, "undef_id")){
        fprintf(stderr, "ID \"");
        printToken(token);
        fprintf(stderr, "\" is used but not defined.");
    }
    else if (!strcmp(err_string, "call_no_f_id")){
        fprintf(stderr, "ID \"");
        printToken(token);
        fprintf(stderr, "\" is not a function ID and therefore cannot be called or used outside of function definitions.");
    }
    else if (!strcmp(err_string, "ret_in_main")){
        fprintf(stderr, "\"");
        printToken(token);
        fprintf(stderr, "\" is a function with returns and therefore cannot be called outside of function definitions.");

    }
    else if (!strcmp(err_string, "write_no_par")){
        fprintf(stderr, "\"write\" function expects at least one argument in call.");
    }
    else if (!strcmp(err_string, "not_enough_pars")){
        fprintf(stderr, "Not enough arguments in function call.");
    }
    else if (!strcmp(err_string, "exp_type")){
        fprintf(stderr, "Expected data type (integer/number/string/nil). ");
        RECEIVED
    }
    else if (!strcmp(err_string, "more_rets_in_def")){
        fprintf(stderr, "Function was declared with less returns than is defined here.");
    }
    else if (!strcmp(err_string, "diff_rets_in_def")){
        fprintf(stderr, "Function was declared with different return types than is defined here.");
    }
    else if (!strcmp(err_string, "less_rets_in_def")){
        fprintf(stderr, "Function was declared with more returns than is defined here.");
    }
    /*else if (!strcmp(err_string, "redef_id")){
        
    }
    else if (!strcmp(err_string, )){

    }*/

    
    fprintf(stderr, "\n");
}

void printToken(token *token){
    if (token->type == TOKEN_ID ||
        token->type == TOKEN_Num || 
        token->type == TOKEN_String || 
        token->type == TOKEN_Int || 
        token->type == TOKEN_Err)
    {
        fprintf(stderr,"%s", token->content);
    }else{
        switch (token->type){
            case TOKEN_Key_do:
                fprintf(stderr,"do");
                break;
            case TOKEN_Key_else:
                fprintf(stderr,"else");
                break;
            case TOKEN_Key_end:
                fprintf(stderr,"end");
                break;
            case TOKEN_Key_function:
                fprintf(stderr,"function");
                break;
            case TOKEN_Key_global:
                fprintf(stderr,"global");
                break;
            case TOKEN_Key_if:
                fprintf(stderr,"if");
                break;
            case TOKEN_Key_integer:
                fprintf(stderr,"integer");
                break;
            case TOKEN_Key_local:
                fprintf(stderr,"local");
                break;
            case TOKEN_Key_nil:
                fprintf(stderr,"nil");
                break;
            case TOKEN_Key_number:
                fprintf(stderr,"number");
                break;
            case TOKEN_Key_require:
                fprintf(stderr,"require");
                break;
            case TOKEN_Key_return:
                fprintf(stderr,"return");
                break;
            case TOKEN_Key_string:
                fprintf(stderr,"string");
                break;
            case TOKEN_Key_then:
                fprintf(stderr,"then");
                break;
            case TOKEN_Key_while:
                fprintf(stderr,"while");
                break;
            case TOKEN_StrLen:
                fprintf(stderr,"#");
                break;
            case TOKEN_Plus:
                fprintf(stderr,"+");
                break;
            case TOKEN_Minus:
                fprintf(stderr,"-");
                break;
            case TOKEN_Mul:
                fprintf(stderr,"*");
                break;
            case TOKEN_Div:
                fprintf(stderr,"/");
                break;
            case TOKEN_DivInt:
                fprintf(stderr,"//");
                break;
            case TOKEN_Concat:
                fprintf(stderr,"..");
                break;
            case TOKEN_Greater:
                fprintf(stderr,">");
                break;
            case TOKEN_GreatEQ:
                fprintf(stderr,">=");
                break;
            case TOKEN_Less:
                fprintf(stderr,"<");
                break;
            case TOKEN_LessEQ:
                fprintf(stderr,"<=");
                break;
            case TOKEN_Assign:
                fprintf(stderr,"=");
                break;
            case TOKEN_EQ:
                fprintf(stderr,"==");
                break;
            case TOKEN_NotEQ:
                fprintf(stderr,"~=");
                break;
            case TOKEN_Colon:
                fprintf(stderr,":");
                break;
            case TOKEN_Comma:
                fprintf(stderr,",");
                break;
            case TOKEN_LeftPar:
                fprintf(stderr,"(");
                break;
            case TOKEN_RightPar:
                fprintf(stderr,")");
                break;
            case TOKEN_EOF:
                fprintf(stderr,"///End-of-file///");
                break;
            default:
                break;
        }
    }
    
}

// --End of file-