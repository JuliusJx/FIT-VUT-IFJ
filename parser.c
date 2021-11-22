/*
 * File: parser.c
 * c file for top down syntactic and semantic analysis
 * Subject: IFJ
 * Authors: Jakub Julius Smykal, Ondrej Kovac, Martin Talajka, Milan Hrabovsky
 * Year: 2021
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

token *returnToken = NULL;

bool cmpTokType( token *token, int cmpType){
    if(token->type == cmpType)
        return true;
    return false;
}

token *nextToken(){
    token *tmp;
    if (returnToken != NULL){
        tmp = returnToken;
        returnToken = NULL;
    }
    else
        tmp = GetToken();
    if(cmpTokType(tmp, TOKEN_Err)){
        fprintf(stderr,"1"); //TODO - add error code
        return NULL;
    }
    return tmp;
}

void freeToken( token *token){
    if((token->type == TOKEN_ID) || (token->type == TOKEN_String)){
        free(token->content.str);
    }
    free(token);
}

bool pProgram(){
    //cToken == current token
    token *cToken;
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_Key_require)){
        fprintf(stderr,"2"); //TODO - add error code
        freeToken(cToken);
        return false;
    }
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(cmpTokType(cToken, TOKEN_String)){
        if(strcmp(cToken->content.str, "ifj21")){
            fprintf(stderr,"2"); //TODO - add error code
            freeToken(cToken);
            return false;
        }
    }
    else{
        fprintf(stderr,"2"); //TODO - add error code
        freeToken(cToken);
        return false;
    }
    if(!pBody()){
        return false;
    }
    if((cToken = nextToken()) == NULL){
        return false;
    }
    free(cToken);
    return true;
}

bool pBody(){
    token *cToken;
    if((cToken = nextToken()) == NULL){
        return false;
    }
    switch(cToken->type){
        case TOKEN_Key_global:
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_ID)){
                fprintf(stderr,"2"); //TODO - add error code
                freeToken(cToken);
                return false;
            }
            //TODO - add entry to symtable
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_Colon)){
                fprintf(stderr,"2"); //TODO - add error code
                freeToken(cToken);
                return false;
            }
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_Key_function)){
                fprintf(stderr,"2"); //TODO - add error code
                freeToken(cToken);
                return false;
            }
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_LeftPar)){
                fprintf(stderr,"2"); //TODO - add error code
                freeToken(cToken);
                return false;
            }
            if(!pParams())
                return false;
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_RightPar)){
                fprintf(stderr,"2"); //TODO - add error code
                freeToken(cToken);
                return false;
            }
            if(!pReturns())
                return false;
            if(!pBody()){
                return false;
            }
            freeToken(cToken);
            break;

        case TOKEN_Key_function:
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_ID)){
                fprintf(stderr,"2"); //TODO - add error code
                freeToken(cToken);
                return false;
            }
            //Add entry to symtable
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_LeftPar)){
                fprintf(stderr,"2"); //TODO - add error code
                freeToken(cToken);
                return false;
            }
            if(!pArgs())
                return false;
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_RightPar)){
                fprintf(stderr,"2"); //TODO - add error code
                freeToken(cToken);
                return false;
            }
            if(!pReturns())
                return false;
            if(!pStatement())
                return false;
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_Key_end)){
                fprintf(stderr,"2"); //TODO - add error code
                freeToken(cToken);
                return false;
            }
            if(!pBody()){
                return false;
            }
            freeToken(cToken);
            break;

        case TOKEN_ID:
            //symtable check
            if(!pAfter_ID())
                return false;
            if(!pBody()){
                return false;
            }
            freeToken(cToken);
            break;
        default: 
            returnToken = cToken;
        break;
    }
    return true;
}

bool pCall(){
    token *cToken;
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_ID)){
        fprintf(stderr,"2"); //TODO - add error code
        freeToken(cToken);
        return false;
    }
    //Symtable check
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_LeftPar)){
        fprintf(stderr,"2"); //TODO - add error code
        freeToken(cToken);
        return false;
    }
    if(!pArgs())
        return false;
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_RightPar)){
        fprintf(stderr,"2"); //TODO - add error code
        freeToken(cToken);
        return false;
    }
    reeToken(cToken);
    return true;
}

bool pParams(){
    token *cToken;
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(cmpTokType(cToken, TOKEN_RightPar)){
        returnToken = cToken;
        return true;
    }
    returnToken = cToken;
    if(!pType())
        return false;
    if(!pParam())
        return false;
    return true;
}

bool pParam(){
    token *cToken;
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_Comma)){
        returnToken = cToken;
        return true;
    }
    else{
        if(!pType())
            return false;
        if(!pParam())
            return false;
    }
    return true;
}

bool pReturns(){
    token *cToken;
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_Colon)){
        returnToken = cToken;
        return true;
    }
    if(!pType())
        return false;
    if(!pParam())
        return false;
    return true;
}

bool pArgs(){
    token *cToken;
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(cmpTokType(cToken, TOKEN_RightPar)){
        returnToken = cToken;
        return true;
    }
    if(!cmpTokType(cToken, TOKEN_ID)){
        fprintf(stderr,"2"); //TODO - add error code
        freeToken(cToken);
        return false;
    }
    //Symtable check
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_Colon)){
        fprintf(stderr,"2"); //TODO - add error code
        freeToken(cToken);
        return false;
    }
    if(!pType())
        return false;
    if(!pArg())
        return false;
    return true;
}

bool pArg(){
    token *cToken;
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_Comma)){
        returnToken = cToken;
        return true;
    }
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_ID)){
        fprintf(stderr,"2"); //TODO - add error code
        freeToken(cToken);
        return false;
    }
    //Symtable check
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_Colon)){
        fprintf(stderr,"2"); //TODO - add error code
        freeToken(cToken);
        return false;
    }
    if(!pType())
        return false;
    if(!pArg())
        return false;
    return true;
}

bool pStatement(){
    token *cToken;
    if((cToken = nextToken()) == NULL){
        return false;
    }
    switch(cToken->type){
        case TOKEN_Key_if:
            //call other parser
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_Key_then)){
                fprintf(stderr,"2"); //TODO - add error code
                freeToken(cToken);
                return false;
            }
            if(!pStatement())
                return false;
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_Key_else)){
                fprintf(stderr,"2"); //TODO - add error code
                freeToken(cToken);
                return false;
            }
            if(!pStatement())
                return false;
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_Key_end)){
                fprintf(stderr,"2"); //TODO - add error code
                freeToken(cToken);
                return false;
            }
            if(!pStatement())
                return false;
            break;

        case TOKEN_Key_while:
            //call other parser
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_Key_do)){
                fprintf(stderr,"2"); //TODO - add error code
                freeToken(cToken);
                return false;
            }
            if(!pStatement())
                return false;
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_Key_end)){
                fprintf(stderr,"2"); //TODO - add error code
                freeToken(cToken);
                return false;
            }
            if(!pStatement())
                return false;
            break;
        
        case TOKEN_Key_local:
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_ID)){
                fprintf(stderr,"2"); //TODO - add error code
                freeToken(cToken);
                return false;
            }
            //Add entry to symtable
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_Colon)){
                fprintf(stderr,"2"); //TODO - add error code
                freeToken(cToken);
                return false;
            }
            if(!pType())
                return false;
            if(!pInit())
                return false;
            if(!pStatement())
                return false;
            break;

        case TOKEN_ID:
            //symtable check
            if(!pAfter_ID())
                return false;
            if(!pStatement())
                return false;
            break;

        case TOKEN_Key_return:
            //call other parser
            break;

        default: 
            returnToken = cToken;
            break;
    }
    return true;
}

bool pAfter_ID(){
    token *cToken;
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(cmpTokType(cToken, TOKEN_LeftPar)){
        if(!pArgs)
            return false;
        if((cToken = nextToken()) == NULL){
            return false;
        }
        if(!cmpTokType(cToken, TOKEN_RightPar)){
            fprintf(stderr,"2"); //TODO - add error code
            freeToken(cToken);
            return false;
        }
    }
    if(!pID())
        return false;
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_Assign)){
        fprintf(stderr,"2"); //TODO - add error code
        freeToken(cToken);
        return false;
    }
    if(!pAfter_EQ)
        return false;
    return true;
}

bool pAfter_EQ(){
    token *cToken;
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(cmpTokType(cToken, TOKEN_ID)){
        //symtable check TODO
        //if function -> call <call>
        //else call other parser
    }
    return true;
}

bool pInit(){
    token *cToken;
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(cmpTokType(cToken, TOKEN_ID)){
        //symtable check TODO
        //if function -> call <call>
        //else call other parser
    }
    else
        returnToken = cToken;
    return true;
}

bool pID(){
    token *cToken;
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_Comma)){
        returnToken = cToken;
        return true;
    }
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_ID)){
        fprintf(stderr,"2"); //TODO - add error code
        freeToken(cToken);
        return false;
    }
    if(!pID())
        return false;
    return true;
}

bool pType(){
    
}

// --End of file--