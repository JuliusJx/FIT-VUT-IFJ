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
symTable *table;
stack *argStack;
symstack *symStack;
char *tokenID = NULL; //maybe rename to functionID
tableItem *callFuncID = NULL;
int scope = 0;

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
        errPrint(1, tmp);
        freeToken(tmp);
        return NULL;
    }
    return tmp;
}

void freeToken( token *token){
    if((token->type == TOKEN_ID) || (token->type == TOKEN_String) || (token->type == TOKEN_Err)){
        if(tokenID != token->content.str)
            free(token->content.str);
    }
    free(token);
}

bool insertBuiltIn(){
    tableItem *item;
    if(!symInsert(table, "reads", FUNC_ID, true, 0)){
        fprintf(stderr,"99"); //TODO - add error code
        errCode = 99; //TODO - add err message
        return false;
    }
    item = symGetItem(table, "reads", 0);
    if(!symAddReturn(item, TYPE_STR)){
        fprintf(stderr,"99"); //TODO - add error code
        errCode = 99; //TODO - add err message
        return false;
    }
    
    if(!symInsert(table, "readi", FUNC_ID, true, 0)){
        fprintf(stderr,"99"); //TODO - add error code
        errCode = 99; //TODO - add err message
        return false;
    }
    item = symGetItem(table, "readi", 0);
    if(!symAddReturn(item, TYPE_INT)){
        fprintf(stderr,"99"); //TODO - add error code
        errCode = 99; //TODO - add err message
        return false;
    }
    
    if(!symInsert(table, "readn", FUNC_ID, true, 0)){
        fprintf(stderr,"99"); //TODO - add error code
        errCode = 99; //TODO - add err message
        return false;
    }
    item = symGetItem(table, "readn", 0);
    if(!symAddReturn(item, TYPE_NUM)){
        fprintf(stderr,"99"); //TODO - add error code
        errCode = 99; //TODO - add err message
        return false;
    }
    
    if(!symInsert(table, "write", FUNC_ID, true, 0)){
        fprintf(stderr,"99"); //TODO - add error code
        errCode = 99; //TODO - add err message
        return false;
    }

    if(!symInsert(table, "tointeger", FUNC_ID, true, 0)){
        fprintf(stderr,"99"); //TODO - add error code
        errCode = 99; //TODO - add err message
        return false;
    }
    item = symGetItem(table, "tointeger", 0);
    if(!symAddParam(item, TYPE_NUM)){
        fprintf(stderr,"99"); //TODO - add error code
        errCode = 99; //TODO - add err message
        return false;
    }
    if(!symAddReturn(item, TYPE_INT)){
        fprintf(stderr,"99"); //TODO - add error code
        errCode = 99; //TODO - add err message
        return false;
    }

    if(!symInsert(table, "substr", FUNC_ID, true, 0)){
        fprintf(stderr,"99"); //TODO - add error code
        errCode = 99; //TODO - add err message
        return false;
    }
    item = symGetItem(table, "substr", 0);
    if(!symAddParam(item, TYPE_STR)){
        fprintf(stderr,"99"); //TODO - add error code
        errCode = 99; //TODO - add err message
        return false;
    }
    if(!symAddParam(item, TYPE_NUM)){
        fprintf(stderr,"99"); //TODO - add error code
        errCode = 99; //TODO - add err message
        return false;
    }
    if(!symAddParam(item, TYPE_NUM)){
        fprintf(stderr,"99"); //TODO - add error code
        errCode = 99; //TODO - add err message
        return false;
    }
    if(!symAddReturn(item, TYPE_STR)){
        fprintf(stderr,"99"); //TODO - add error code
        errCode = 99; //TODO - add err message
        return false;
    }
    
    if(!symInsert(table, "ord", FUNC_ID, true, 0)){
        fprintf(stderr,"99"); //TODO - add error code
        errCode = 99; //TODO - add err message
        return false;
    }
    item = symGetItem(table, "ord", 0);
    if(!symAddParam(item, TYPE_STR)){
        fprintf(stderr,"99"); //TODO - add error code
        errCode = 99; //TODO - add err message
        return false;
    }
    if(!symAddParam(item, TYPE_INT)){
        fprintf(stderr,"99"); //TODO - add error code
        errCode = 99; //TODO - add err message
        return false;
    }
    if(!symAddReturn(item, TYPE_INT)){
        fprintf(stderr,"99"); //TODO - add error code
        errCode = 99; //TODO - add err message
        return false;
    }
    
    if(!symInsert(table, "chr", FUNC_ID, true, 0)){
        fprintf(stderr,"99"); //TODO - add error code
        errCode = 99; //TODO - add err message
        return false;
    }
    item = symGetItem(table, "chr", 0);
    if(!symAddParam(item, TYPE_INT)){
        fprintf(stderr,"99"); //TODO - add error code
        errCode = 99; //TODO - add err message
        return false;
    }
    if(!symAddReturn(item, TYPE_STR)){
        fprintf(stderr,"99"); //TODO - add error code
        errCode = 99; //TODO - add err message
        return false;
    }

    return true;
}


bool pProgram(){
    //cToken == current token
    token *cToken;
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_Key_require)){
        fprintf(stderr,"2a"); //TODO - add error code
        errPrint(2, cToken);
        freeToken(cToken);
        return false;
    }
    
    freeToken(cToken);
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(cmpTokType(cToken, TOKEN_String)){
        if(strcmp(cToken->content.str, "ifj21")){
            fprintf(stderr,"2b"); //TODO - add error code
            errPrint(2, cToken);
            freeToken(cToken);
            return false;
        }
    }
    else{
        fprintf(stderr,"2c"); //TODO - add error code
        errPrint(2, cToken);
        freeToken(cToken);
        return false;
    }
    //this sounds better than free(null) lol
    tokenID = cToken->content.str;
    freeToken(cToken);
    //scope 0 represents scope, where all functions are defined
    scope = 0;
    if(!pBody()){
        return false;
    }
    
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_EOF)){
        fprintf(stderr,"2asd"); //TODO - add error code
        errPrint(2, cToken);
        freeToken(cToken);
        return false;
    }
    freeToken(cToken);
    return true;
}

bool pBody(){
    token *cToken;
    if((cToken = nextToken()) == NULL){
        return false;
    }
    tableItem *item;
    switch(cToken->type){
        case TOKEN_Key_global:
            
            freeToken(cToken);
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_ID)){
                fprintf(stderr,"2d"); //TODO - add error code
                errPrint(2, cToken);
                freeToken(cToken);
                return false;
            }
            if(symGetItem(table, cToken->content.str, 0) != NULL){
                fprintf(stderr,"3a"); //TODO - add error code
                errPrint(3, cToken);
                freeToken(cToken);
                return false;
            }
            free(tokenID);
            tokenID = cToken->content.str;
            if(!symInsert(table, cToken->content.str, FUNC_ID, false, 0)){
                fprintf(stderr,"99t"); //TODO - add error code
                errPrint(99, cToken);
                freeToken(cToken);
                return false;
            }

            freeToken(cToken);
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_Colon)){
                fprintf(stderr,"2e"); //TODO - add error code
                errPrint(2, cToken);
                freeToken(cToken);
                return false;
            }
            
            freeToken(cToken);
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_Key_function)){
                fprintf(stderr,"2f"); //TODO - add error code
                errPrint(2, cToken);
                freeToken(cToken);
                return false;
            }
            
            freeToken(cToken);
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_LeftPar)){
                fprintf(stderr,"2g"); //TODO - add error code
                errPrint(2, cToken);
                freeToken(cToken);
                return false;
            }
            
            freeToken(cToken);
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if((cmpTokType(cToken, TOKEN_Key_string)) || (cmpTokType(cToken, TOKEN_Key_integer)) || (cmpTokType(cToken, TOKEN_Key_number))){
                returnToken = cToken;
                if(!pParams())
                    return false;
            }
            else{
                returnToken = cToken;
            }
            
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_RightPar)){
                fprintf(stderr,"2h"); //TODO - add error code
                errPrint(2, cToken);
                freeToken(cToken);
                return false;
            }
            
            freeToken(cToken);
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(cmpTokType(cToken, TOKEN_Colon)){
                freeToken(cToken);
                if(!pReturns())
                    return false;
            }
            else{
                returnToken = cToken;
            }
            
            if(!pBody()){
                return false;
            }
            break;

        case TOKEN_Key_function:
            
            freeToken(cToken);
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_ID)){
                fprintf(stderr,"2i"); //TODO - add error code
                errPrint(2, cToken);
                freeToken(cToken);
                return false;
            }
            item = symGetItem(table, cToken->content.str, 0);
            if(item != NULL){
                if(item->isInit){
                    fprintf(stderr,"3b"); //TODO - add error code
                    errPrint(3, cToken);
                    freeToken(cToken);
                    return false;
                }
                symToggleInit(table, cToken->content.str, 0);
            }
            else{
                symInsert(table, cToken->content.str, FUNC_ID, false, 0);
            }
            free(tokenID);
            tokenID = cToken->content.str;
            freeToken(cToken);

            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_LeftPar)){
                fprintf(stderr,"2j"); //TODO - add error code
                errPrint(2, cToken);
                freeToken(cToken);
                return false;
            }
            freeToken(cToken);

            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_RightPar)){
                returnToken = cToken;
                scope++;
                if(!pArgs())
                    return false;
                scope--;
            }
            else{
                returnToken = cToken;
            }
            
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_RightPar)){
                fprintf(stderr,"2k"); //TODO - add error code
                errPrint(2, cToken);
                freeToken(cToken);
                return false;
            }
            item = symGetItem(table, tokenID, 0);
            freeToken(cToken);

            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(cmpTokType(cToken, TOKEN_Colon)){
                freeToken(cToken);
                if(!pReturns())
                    return false;
            }
            else{
                if(item->returnAmount > 0){
                    fprintf(stderr,"3c"); //TODO - add error code
                    errPrint(3, cToken);
                    freeToken(cToken);
                    return false;
                }
                returnToken = cToken;
            }
            
            symToggleInit(table, tokenID, 0);
            scope++;
            if(!pStatement())
                return false;
            //TODO maybe add free
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_Key_end)){
                fprintf(stderr,"2l"); //TODO - add error code
                errPrint(2, cToken);
                freeToken(cToken);
                return false;
            }
            symDeleteScope(table, scope);
            scope--;
            freeToken(cToken);
            if(!pBody()){
                return false;
            }
            break;

        case TOKEN_ID:
            
            returnToken = cToken;
            if(!pCall())
                return false;
            if(!pBody()){
                return false;
            }
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
        fprintf(stderr,"2m"); //TODO - add error code
        errPrint(2, cToken);
        freeToken(cToken);
        return false;
    }
    tableItem *item = symGetItem(table, cToken->content.str, 0);
    if(item == NULL){
        fprintf(stderr,"3d"); //TODO - add error code
        errPrint(3, cToken);
        freeToken(cToken);
        return false;
    }
    if(item->type != FUNC_ID){
        fprintf(stderr,"3e"); //TODO - add error code
        errPrint(3, cToken);
        freeToken(cToken);
        return false;
    }
    callFuncID = item;
    symToggleUsed(table, cToken->content.str, 0); //all functions are defined at scope 0
    int i = item->paramAmount - 1;
    while (i >= 0){
        stackPush(argStack, item->paramTypes[i--] - '0');
    }
    freeToken(cToken);

    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_LeftPar)){
        fprintf(stderr,"2n"); //TODO - add error code
        errPrint(2, cToken);
        freeToken(cToken);
        return false;
    }
    freeToken(cToken);

    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(cmpTokType(cToken, TOKEN_ID) || cmpTokType(cToken, TOKEN_String) || cmpTokType(cToken, TOKEN_Int) || cmpTokType(cToken, TOKEN_Num)){
        returnToken = cToken;
        if(!pCallArgs())
            return false;
    }
    else{
        if(!strcmp(item->name, "write")){
            fprintf(stderr,"5"); //TODO - add error code
            errPrint(5, cToken);
            freeToken(cToken);
            return false;
        }
        if(item->paramAmount > 0){
            fprintf(stderr,"5a"); //TODO - add error code
            errPrint(5, cToken);
            freeToken(cToken);
            return false;
        }
        returnToken = cToken;
    }
    
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_RightPar)){
        fprintf(stderr,"2o"); //TODO - add error code
        errPrint(2, cToken);
        freeToken(cToken);
        return false;
    }
    freeToken(cToken);
    return true;
}

bool pParams(){
    token *cToken;
    if((cToken = nextToken()) == NULL){
        return false;
    }

    int type;
    switch(cToken->type){
        
        case TOKEN_Key_string:
            type = TYPE_STR;
            break;
        
        case TOKEN_Key_integer:
            type = TYPE_INT;
            break;

        case TOKEN_Key_number:
            type = TYPE_NUM;
            break;
        default:
            fprintf(stderr,"2p"); //TODO - add error code
            errPrint(2, cToken);
            free(cToken);
            return false;
    }
    tableItem *item = symGetItem(table, tokenID, 0);
    if(!symAddParam(item, type)){
        fprintf(stderr,"99"); //TODO - add error code
        errPrint(99, cToken);
        freeToken(cToken);
        return false;
    }
    freeToken(cToken);

    //if next token is comma, recursive call
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_Comma)){
        returnToken = cToken;
        return true;
    }
    else
        freeToken(cToken);
    
    if(!pParams())
        return false;
    return true;
}

bool pReturns(){
    token *cToken;
    if((cToken = nextToken()) == NULL){
        return false;
    }
    static int returnIndex = 0;
    
    int type;
    switch(cToken->type){
        
        case TOKEN_Key_string:
            type = TYPE_STR;
            break;
        
        case TOKEN_Key_integer:
            type = TYPE_INT;
            break;

        case TOKEN_Key_number:
            type = TYPE_NUM;
            break;
        default:
            fprintf(stderr,"2q"); //TODO - add error code
            errPrint(2, cToken);
            free(cToken);
            return false;
    }
    tableItem *item = symGetItem(table, tokenID, 0);
    if(item->isInit){
        if(returnIndex >= item->returnAmount){
            fprintf(stderr,"3f"); //TODO - add error code
            errPrint(3, cToken);
            free(cToken);
            return false;
        }
        if(type != item->returnTypes[returnIndex++] - '0'){
            fprintf(stderr,"3g"); //TODO - add error code
            errPrint(3, cToken);
            free(cToken);
            return false;
        }
    }
    else{
        if(!symAddReturn(item, type)){
            fprintf(stderr,"99"); //TODO - add error code
            errPrint(99, cToken);
            freeToken(cToken);
            return false;
        }
    }
    freeToken(cToken);

    //if next token is comma, recursive call
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_Comma)){
        returnToken = cToken;
        returnIndex = 0;
        return true;
    }
    else
        freeToken(cToken);
    
    if(!pReturns())
        return false;
    return true;
}

bool pArgs(){
    
    token *cToken;
    static int paramIndex = 0;
    //looks for function entry
    tableItem *item = symGetItem(table, tokenID, 0);
    
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_ID)){
        fprintf(stderr,"2r"); //TODO - add error code
        errPrint(2, cToken);
        freeToken(cToken);
        return false;
    }
    if(symGetItem(table, cToken->content.str, scope) != NULL){
        fprintf(stderr,"3h"); //TODO - add error code
        errPrint(3, cToken);
        freeToken(cToken);
        return false;
    }
    
    char *tmp = cToken->content.str; //maybe future error TODO something
    free(cToken); //freeToken() would also free content.str, which we need in tmp

    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_Colon)){
        fprintf(stderr,"2s"); //TODO - add error code
        errPrint(2, cToken);
        freeToken(cToken);
        return false;
    }
    
    freeToken(cToken);
    if((cToken = nextToken()) == NULL){
        return false;
    }
    
    int type;
    switch(cToken->type){
        
        case TOKEN_Key_string:
            type = TYPE_STR;
            break;
        
        case TOKEN_Key_integer:
            type = TYPE_INT;
            break;

        case TOKEN_Key_number:
            type = TYPE_NUM;
            break;
        default:
            fprintf(stderr,"2t"); //TODO - add error code
            errPrint(2, cToken);
            free(cToken);
            return false;
    }

    if(item->isInit){
        if(paramIndex >= item->paramAmount){
            fprintf(stderr,"3i"); //TODO - add error code
            errPrint(3, cToken);
            free(cToken);
            return false;
        }
        if(type != item->returnTypes[paramIndex++] - '0'){
            fprintf(stderr,"3j"); //TODO - add error code
            errPrint(3, cToken);
            free(cToken);
            return false;
        }
    }
    else{
        if(!symAddParam(item, type)){
            fprintf(stderr,"99"); //TODO - add error code
            errPrint(99, cToken);
            freeToken(cToken);
            return false;
        }
    }
    symInsert(table, tmp, type, true, scope);
    freeToken(cToken);

    //if next token is comma, recursive call
    if((cToken = nextToken()) == NULL){
        return false;
    }
    free(tmp);
    if(!cmpTokType(cToken, TOKEN_Comma)){
        returnToken = cToken;
        paramIndex = 0;
        return true;
    }
    else
        freeToken(cToken);

    if(!pArgs())
        return false;
    return true;
}

bool pCallArgs(){
    token *cToken;

    if((cToken = nextToken()) == NULL){
        return false;
    }
    int type;
    if(!cmpTokType(cToken, TOKEN_ID)){
        if(cmpTokType(cToken, TOKEN_String) || cmpTokType(cToken, TOKEN_Int) || cmpTokType(cToken, TOKEN_Num)){
            if(!strcmp(callFuncID->name,"write")){
                if(cToken->type == TOKEN_String){
                    fprintf(stderr,"5bxx"); //TODO - add error code
                    errPrint(5, cToken);
                    freeToken(cToken);
                    return false;
                }
            }
            else{
                if(!stackPop(argStack, &type)){ //more params than expected
                    fprintf(stderr,"5bxy"); //TODO - add error code
                    errPrint(5, cToken);
                    freeToken(cToken);
                    return false;
                }
                switch (type){
                    case TYPE_STR:
                        type = TOKEN_String;
                        break;
                    case TYPE_INT:
                        type = TOKEN_Int;
                        break;
                    case TYPE_NUM:
                        type = TOKEN_Num;
                        break;
                    default: break;
                }

                if(cToken->type != type){
                    fprintf(stderr,"4x"); //TODO - add error code
                    errPrint(4, cToken);
                    freeToken(cToken);
                    return false;
                }
            }
        }
        else{
            fprintf(stderr,"2u"); //TODO - add error code
            errPrint(2, cToken);
            freeToken(cToken);
            return false;
        }
    }
    else{
        tableItem *item = symGetItem(table, cToken->content.str, scope);
        if(item == NULL){
            fprintf(stderr,"3k"); //TODO - add error code
            errPrint(3, cToken);
            freeToken(cToken);
            return false;
        }
        if(!strcmp(callFuncID->name,"write")){
            if((item->type == TYPE_INT) || (item->type == TYPE_NUM)){
                type = item->type;
            }
            else{
                fprintf(stderr,"5bx"); //TODO - add error code
                errPrint(5, cToken);
                freeToken(cToken);
                return false;
            }
        }
        else{
            if(!stackPop(argStack, &type)){ //more params than expected
                fprintf(stderr,"5b"); //TODO - add error code
                errPrint(5, cToken);
                freeToken(cToken);
                return false;
            }
        }
        if(item->type != type){
            fprintf(stderr,"4"); //TODO - add error code
            errPrint(4, cToken);
            freeToken(cToken);
            return false;
        }
    }
    
    freeToken(cToken);

    //if next token is comma, recursive call
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_Comma)){
        returnToken = cToken;
        if(!stackIsEmpty(argStack)){ //less params than expected
            fprintf(stderr,"5c"); //TODO - add error code
            errPrint(5, cToken);
            freeToken(cToken);
            return false;
        }
        return true;
    }
    else
        freeToken(cToken);
    
    if(!pCallArgs())
        return false;
    return true;
}

bool pStatement(){
    token *cToken;
    if((cToken = nextToken()) == NULL){
        return false;
    }
    tableItem *item;
    
    int index;
    switch(cToken->type){
        case TOKEN_Key_if:

            freeToken(cToken);
            if(!pExpression())
                return false;
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_Key_then)){
                fprintf(stderr,"2v"); //TODO - add error code
                errPrint(2, cToken);
                freeToken(cToken);
                return false;
            }
            scope++;
            if(!pStatement())
                return false;
            
            symDeleteScope(table, scope);
            scope--;
            freeToken(cToken);

            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_Key_else)){
                fprintf(stderr,"2w"); //TODO - add error code
                errPrint(2, cToken);
                freeToken(cToken);
                return false;
            }

            scope++;
            if(!pStatement())
                return false;
            symDeleteScope(table, scope);
            scope--;
            freeToken(cToken);

            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_Key_end)){
                fprintf(stderr,"2x"); //TODO - add error code
                errPrint(2, cToken);
                freeToken(cToken);
                return false;
            }
            freeToken(cToken);
            if(!pStatement())
                return false;
            break;

        case TOKEN_Key_while:
            
            freeToken(cToken);
            if(!pExpression())
                return false;

            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_Key_do)){
                fprintf(stderr,"2y"); //TODO - add error code
                errPrint(2, cToken);
                freeToken(cToken);
                return false;
            }

            scope++;
            if(!pStatement())
                return false;
            symDeleteScope(table, scope);
            scope--;
            freeToken(cToken);

            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_Key_end)){
                fprintf(stderr,"2z"); //TODO - add error code
                errPrint(2, cToken);
                freeToken(cToken);
                return false;
            }
            freeToken(cToken);
            if(!pStatement())
                return false;
            break;
        
        case TOKEN_Key_local:

            freeToken(cToken);
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_ID)){
                fprintf(stderr,"2aa"); //TODO - add error code
                errPrint(2, cToken);
                freeToken(cToken);
                return false;
            }
            //Add entry to symtable
            item = symGetItem(table, cToken->content.str, scope);
            if(item != NULL){
                if((item->scope == scope) || (item->type == FUNC_ID)){
                    fprintf(stderr,"3l"); //TODO - add error code
                    errPrint(3, cToken);
                    freeToken(cToken);
                    return false;
                }
            }
            char *tmp = cToken->content.str;
            free(cToken);

            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_Colon)){
                fprintf(stderr,"2bb"); //TODO - add error code
                errPrint(2, cToken);
                freeToken(cToken);
                return false;
            }
            freeToken(cToken);

            if((cToken = nextToken()) == NULL){
                return false;
            }
            int type;
            switch(cToken->type){
                
                case TOKEN_Key_string:
                    type = TYPE_STR;
                    break;
                
                case TOKEN_Key_integer:
                    type = TYPE_INT;
                    break;

                case TOKEN_Key_number:
                    type = TYPE_NUM;
                    break;
                default:
                    fprintf(stderr,"2cc"); //TODO - add error code
                    errPrint(2, cToken);
                    free(cToken);
                    return false;
            }
            if(!symInsert(table, tmp, type, false, scope)){
                fprintf(stderr,"99"); //TODO - add error code
                errPrint(99, cToken);
                freeToken(cToken);
                return false;
            }
            freeToken(cToken);

            item = symGetItem(table, tmp, scope);
            symstackPush(symStack, item);
            if(!pInit())
                return false;
            symstackPop(symStack, &item);
            
            free(tmp);
            if(!pStatement())
                return false;
            break;

        case TOKEN_ID:
            
            item = symGetItem(table, cToken->content.str, scope);
            if(item == NULL){
                fprintf(stderr,"3m"); //TODO - add error code
                errPrint(3, cToken);
                freeToken(cToken);
                return false;
            }
            if(item->type == FUNC_ID){
                returnToken = cToken;
                if(!pCall())
                    return false;
            }
            else{
                symstackPush(symStack, item);
                freeToken(cToken);

                if((cToken = nextToken()) == NULL)
                    return false;
                if(cmpTokType(cToken, TOKEN_Comma)){
                    freeToken(cToken);
                    if(!pID())
                        return false;
                }
                else{
                    returnToken = cToken;
                }
                
                if((cToken = nextToken()) == NULL)
                    return false;
                if(cmpTokType(cToken, TOKEN_Assign)){
                    freeToken(cToken);

                    if((cToken = nextToken()) == NULL)
                        return false;
                    if(cmpTokType(cToken, TOKEN_ID)){
                        item = symGetItem(table, cToken->content.str, scope);
                        if(item == NULL){
                            fprintf(stderr,"3n"); //TODO - add error code
                            errPrint(3, cToken);
                            freeToken(cToken);
                            return false;
                        }
                        if(item->type == FUNC_ID){
                            index = symstackCount(symStack);
                            if(index > item->returnAmount){
                                fprintf(stderr,"5d"); //TODO - add error code
                                errPrint(5, cToken);
                                freeToken(cToken);
                                return false;
                            }
                            tableItem *tmp;
                            while(!symstackIsEmpty(symStack)){
                                symstackPop(symStack, &tmp);
                                if(tmp->type != item->returnTypes[--index] - '0'){
                                    fprintf(stderr,"5e"); //TODO - add error code
                                    errPrint(5, cToken);
                                    freeToken(cToken);
                                    return false;
                                }
                            }
                            returnToken = cToken;
                            if(!pCall()){
                                return false;
                            }
                        }
                        else{
                            returnToken = cToken;
                            if(!pExpression())
                                return false;
                        }
                    }
                    else if (cmpTokType(cToken, TOKEN_Num) || cmpTokType(cToken, TOKEN_Int) || cmpTokType(cToken, TOKEN_String) || cmpTokType(cToken, TOKEN_StrLen) || cmpTokType(cToken, TOKEN_LeftPar)){
                        returnToken = cToken;
                        if(!pExpression())
                            return false;
                    }
                    else{
                        fprintf(stderr,"5f"); //TODO - add error code
                        errPrint(5, cToken);
                        freeToken(cToken);
                        return false;
                    }
                    
                }
                else{
                    fprintf(stderr,"3o"); //TODO - add error code
                    errPrint(3, cToken);
                    freeToken(cToken);
                    return false;
                }
            }
            //TODO - maybe leak here
            if(!pStatement())
                return false;
            break;

        case TOKEN_Key_return:
            index = 0;
            tableItem *item = symGetItem(table, tokenID, 0);
            if(item == NULL){
                fprintf(stderr,"99"); //TODO - add error code
                errPrint(99, cToken);
                freeToken(cToken);
                return false;
            }
            freeToken(cToken);
            while(index < item->returnAmount){
                tableItem newItem = { 
                    .name = "retval",
                    .type = item->returnTypes[index++],
                    .isInit = true,
                    .isUsed = false,
                    .paramAmount = 0,
                    .returnAmount = 0,
                    .paramTypes = NULL,
                    .returnTypes = NULL,
                    .scope = scope,
                    .next = NULL };
                symstackPush(symStack, &newItem);
            }
            if(!pExpression())
                return false;
            break;

        default:
            returnToken = cToken;
            break;
    }
    return true;
}

bool pInit(){
    token *cToken;
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_Assign)){
        returnToken = cToken;
        return true;
    }
    tableItem *item;
    freeToken(cToken);

    if((cToken = nextToken()) == NULL)
        return false;
    if(cmpTokType(cToken, TOKEN_ID)){
        item = symGetItem(table, cToken->content.str, scope);
        if(item->type == FUNC_ID){
            int index = symstackCount(symStack);
            if(index > item->returnAmount){
                fprintf(stderr,"5g"); //TODO - add error code
                errPrint(5, cToken);
                freeToken(cToken);
                return false;
            }
            tableItem *tmp;
            while(!symstackIsEmpty(symStack)){
                symstackPop(symStack, &tmp);
                if(tmp->type != item->returnTypes[--index] - '0'){
                    fprintf(stderr,"5h"); //TODO - add error code
                    errPrint(5, cToken);
                    freeToken(cToken);
                    return false;
                }
            }
            returnToken = cToken;
            if(!pCall())
                return false;
        }
        else{
            returnToken = cToken;
            if(!pExpression())
                return false;
        }
    }
    else if (cmpTokType(cToken, TOKEN_Num) || cmpTokType(cToken, TOKEN_Int) || cmpTokType(cToken, TOKEN_String) || cmpTokType(cToken, TOKEN_StrLen) || cmpTokType(cToken, TOKEN_LeftPar)){
        returnToken = cToken;
        if(!pExpression())
            return false;
    }
    else{
        fprintf(stderr,"5i"); //TODO - add error code
        errPrint(5, cToken);
        freeToken(cToken);
        return false;
    }
    return true;
}

bool pID(){
    token *cToken;
    
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_ID)){
        fprintf(stderr,"2dd"); //TODO - add error code
        errPrint(2, cToken);
        freeToken(cToken);
        return false;
    }
    tableItem *item = symGetItem(table, cToken->content.str, scope);
    if(item->type == FUNC_ID){
        fprintf(stderr,"7"); //TODO - add error code
        errPrint(7, cToken);
        freeToken(cToken);
        return false;
    }

    symstackPush(symStack, item);
    freeToken(cToken);

    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_Comma)){
        returnToken = cToken;
        return true;
    }
    freeToken(cToken);
    if(!pID())
        return false;
    return true;
}

// ---------------
int main(){
    if((table = malloc(sizeof(symTable))) == NULL){
        fprintf(stderr, "ERROR: -- code: %d\n", errCode);
        return errCode;
    }
    if((argStack = malloc(sizeof(stack))) == NULL){
        free(table);
        fprintf(stderr, "ERROR: -- code: %d\n", errCode);
        return errCode;
    }
    if((symStack = malloc(sizeof(symstack))) == NULL){
        free(argStack);
        free(table);
        fprintf(stderr, "ERROR: -- code: %d\n", errCode);
        return errCode;
    }
    symInit(table);
    stackInit(argStack);
    symstackInit(symStack);
    
    insertBuiltIn();
    pProgram();
    int dump;
    tableItem *ptrDump;
    symDeleteAll(table);
    while(!stackIsEmpty(argStack)){
        stackPop(argStack, &dump);
    }
    while(!symstackIsEmpty(symStack)){
        symstackPop(symStack, &ptrDump);
        //free(ptrDump);
    }
    free(table);
    free(argStack);
    free(symStack);
    free(tokenID);
    if (errCode){
        printf("\ntest run failed\n");
    }
    else
        printf("\ntest run successfuly\n");
    return errCode;
}

// --End of file--