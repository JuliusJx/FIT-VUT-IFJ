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
    //scope 0 represents scope, where all functions are defined
    scope = 0;
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
            if(symGetItem(table, cToken->content.str, 0) != NULL){
                fprintf(stderr,"3"); //TODO - add error code
                freeToken(cToken);
                return false;
            }
            tokenID = cToken->content.str;
            symInsert(table, cToken->content.str, FUNC_ID, false, 0);

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

            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(cmpTokType(cToken, TOKEN_ID)){
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
                fprintf(stderr,"2"); //TODO - add error code
                freeToken(cToken);
                return false;
            }

            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(cmpTokType(cToken, TOKEN_Colon)){
                if(!pReturns())
                    return false;
            }
            else{
                returnToken = cToken;
            }
            
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
            tableItem *item;
            item = symGetItem(table, cToken->content.str, 0);
            if(item != NULL){
                if(item->isInit){
                    fprintf(stderr,"3"); //TODO - add error code
                    freeToken(cToken);
                    return false;
                }
                symToggleInit(table, cToken->content.str, 0);
            }
            else{
                symInsert(table, cToken->content.str, FUNC_ID, false, 0);
            }
            tokenID = cToken->content.str;

            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_LeftPar)){
                fprintf(stderr,"2"); //TODO - add error code
                freeToken(cToken);
                return false;
            }

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
                fprintf(stderr,"2"); //TODO - add error code
                freeToken(cToken);
                return false;
            }
            
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(cmpTokType(cToken, TOKEN_Colon)){
                if(!pReturns())
                    return false;
            }
            else{
                returnToken = cToken;
            }

            symToggleInit(table, tokenID, 0);
            scope++;
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
            //remove variables at scope 1
            scope--;
            if(!pBody()){
                return false;
            }
            freeToken(cToken);
            break;

        case TOKEN_ID:
            
            returnToken = cToken;
            if(!pCall())
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
    tableItem *item = symGetItem(table, cToken->content.str, 0);
    if(item == NULL){
        fprintf(stderr,"3"); //TODO - add error code
        freeToken(cToken);
        return false;
    }
    if(item->type != FUNC_ID){
        fprintf(stderr,"3"); //TODO - add error code
        freeToken(cToken);
        return false;
    }
    symToggleUsed(table, cToken->content.str, 0); //all functions are defined at scope 0
    int i = item->paramAmount - 1;
    while (i >= 0){
        stackPush(argStack, item->paramTypes[i--]);
    }
    symToggleUsed(table, cToken->content.str, 0);
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_LeftPar)){
        fprintf(stderr,"2"); //TODO - add error code
        freeToken(cToken);
        return false;
    }

    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(cmpTokType(cToken, TOKEN_ID)){
        returnToken = cToken;
        if(!pCallArgs())
            return false;
    }
    else{
        returnToken = cToken;
    }
    
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_RightPar)){
        fprintf(stderr,"2"); //TODO - add error code
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
        
        case TOKEN_String:
            type = TYPE_STR;
            break;
        
        case TOKEN_Int:
            type = TYPE_INT;
            break;

        case TOKEN_Num:
            type = TYPE_NUM;
            break;
        default:
            fprintf(stderr,"2"); //TODO - add error code
            free(cToken);
            return false;
    }
    tableItem *item = symGetItem(table, tokenID, 0);
    if(!symAddParam(item, type)){
        fprintf(stderr,"99"); //TODO - add error code
        freeToken(cToken);
        return false;
    }
    //if next token is comma, recursive call
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_Comma)){
        returnToken = cToken;
        return true;
    }
    if(!pParams())
        return false;
    return true;
}

bool pReturns(){
    token *cToken;
    static int returnIndex = 0;
    
    int type;
    switch(cToken->type){
        
        case TOKEN_String:
            type = TYPE_STR;
            break;
        
        case TOKEN_Int:
            type = TYPE_INT;
            break;

        case TOKEN_Num:
            type = TYPE_NUM;
            break;
        default:
            fprintf(stderr,"2"); //TODO - add error code
            free(cToken);
            return false;
    }
    tableItem *item = symGetItem(table, tokenID, 0);
    if(item->isInit){
        if(returnIndex >= item->returnAmount){
            fprintf(stderr,"3"); //TODO - add error code
            free(cToken);
            return false;
        }
        if(type != item->returnTypes[returnIndex++]){
            fprintf(stderr,"3"); //TODO - add error code
            free(cToken);
            return false;
        }
    }
    else{
        if(!symAddReturn(item, type)){
            fprintf(stderr,"99"); //TODO - add error code
            freeToken(cToken);
            return false;
        }
    }
    //if next token is comma, recursive call
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_Comma)){
        returnToken = cToken;
        returnIndex = 0;
        return true;
    }
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
        fprintf(stderr,"2"); //TODO - add error code
        freeToken(cToken);
        return false;
    }
    if(symGetItem(table, cToken->content.str, scope) != NULL){
        fprintf(stderr,"3"); //TODO - add error code
        freeToken(cToken);
        return false;
    }

    char *tmp = cToken->content.str; //maybe future error TODO something

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

    int type;
    switch(cToken->type){
        
        case TOKEN_String:
            type = TYPE_STR;
            break;
        
        case TOKEN_Int:
            type = TYPE_INT;
            break;

        case TOKEN_Num:
            type = TYPE_NUM;
            break;
        default:
            fprintf(stderr,"2"); //TODO - add error code
            free(cToken);
            return false;
    }

    if(item->isInit){
        if(paramIndex >= item->paramAmount){
            fprintf(stderr,"3"); //TODO - add error code
            free(cToken);
            return false;
        }
        if(type != item->paramTypes[paramIndex++]){
            fprintf(stderr,"3"); //TODO - add error code
            free(cToken);
            return false;
        }
    }
    else{
        if(!symAddReturn(item, type)){
            fprintf(stderr,"99"); //TODO - add error code
            freeToken(cToken);
            return false;
        }
    }
    symInsert(table, tmp, type, true, scope);
    
    //if next token is comma, recursive call
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_Comma)){
        returnToken = cToken;
        paramIndex = 0;
        return true;
    }
    if(!pArgs())
        return false;
    return true;
}

bool pCallArgs(){
    token *cToken;
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_ID)){
        fprintf(stderr,"2"); //TODO - add error code
        freeToken(cToken);
        return false;
    }
    tableItem *item = symGetItem(table, cToken->content.str, scope);
    if(item == NULL){
        fprintf(stderr,"3"); //TODO - add error code
        freeToken(cToken);
        return false;
    }
    int type;
    if(!stackPop(argStack, &type)){ //more params than expected
        fprintf(stderr,"5"); //TODO - add error code
        freeToken(cToken);
        return false;
    }
    if(item->type != type){
        fprintf(stderr,"4"); //TODO - add error code
        freeToken(cToken);
        return false;
    }
    //if next token is comma, recursive call
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_Comma)){
        returnToken = cToken;
        if(!stackIsEmpty(argStack)){ //less params than expected
            fprintf(stderr,"5"); //TODO - add error code
            freeToken(cToken);
            return false;
        }
        return true;
    }
    if(!pCallArgs())
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
            scope++;
            if(!pStatement())
                return false;
            //remove variables at scope
            scope--;

            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(!cmpTokType(cToken, TOKEN_Key_else)){
                fprintf(stderr,"2"); //TODO - add error code
                freeToken(cToken);
                return false;
            }

            scope++;
            if(!pStatement())
                return false;
            //remove variables at scope
            scope--;

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

            scope++;
            if(!pStatement())
                return false;
            //TODO remove all variables at scope
            scope--;
            
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
            tableItem *item = symGetItem(table, cToken->content.str, scope);
            if(item != NULL){
                if((item->scope == scope) || (item->type == FUNC_ID)){
                    fprintf(stderr,"3"); //TODO - add error code
                    freeToken(cToken);
                    return false;
                }
            }
            char *tmp = cToken->content.str;

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
            int type;
            switch(cToken->type){
                
                case TOKEN_String:
                    type = TYPE_STR;
                    break;
                
                case TOKEN_Int:
                    type = TYPE_INT;
                    break;

                case TOKEN_Num:
                    type = TYPE_NUM;
                    break;
                default:
                    fprintf(stderr,"2"); //TODO - add error code
                    free(cToken);
                    return false;
            }
            if(!symInsert(table, tmp, type, false, scope)){
                fprintf(stderr,"99"); //TODO - add error code
                freeToken(cToken);
                return false;
            }
            item = symGetItem(table, cToken->content.str, scope);
            symstackPush(symStack, item);
            if(!pInit())
                return false;
            symstackPop(symStack, item);
            if(!pStatement())
                return false;
            break;

        case TOKEN_ID:
            tableItem *item = symGetItem(table, cToken->content.str, scope);
            if(item == NULL){
                fprintf(stderr,"3"); //TODO - add error code
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
                
                if((cToken = nextToken()) == NULL)
                    return false;
                if(cmpTokType(cToken, TOKEN_Colon)){
                    if(!pID())
                        return false;
                }
                else if(cmpTokType(cToken, TOKEN_Assign)){

                    if((cToken = nextToken()) == NULL)
                        return false;
                    if(cmpTokType(cToken, TOKEN_ID)){
                        item = symGetItem(table, cToken->content.str, scope);
                        if(item->type == FUNC_ID){
                            int index = symstackCount(symStack);
                            if(index > item->returnAmount){
                                fprintf(stderr,"5"); //TODO - add error code
                                freeToken(cToken);
                                return false;
                            }
                            symstackItem *tmp;
                            while(!symstackIsEmpty(symStack)){
                                symstackPop(symStack, tmp);
                                if(tmp->item->type != item->returnTypes[--index]){
                                    fprintf(stderr,"5"); //TODO - add error code
                                    freeToken(cToken);
                                    return false;
                                }
                            }
                        }
                        else{
                            //call other parser
                        }
                    }
                    else if (cmpTokType(cToken, TOKEN_Num) || cmpTokType(cToken, TOKEN_Int) || cmpTokType(cToken, TOKEN_String)){
                        //call other parser
                    }
                    else{
                        fprintf(stderr,"5"); //TODO - add error code
                        freeToken(cToken);
                        return false;
                    }
                    
                }
                else{
                    fprintf(stderr,"3"); //TODO - add error code
                    freeToken(cToken);
                    return false;
                }
            }
            if(!pStatement())
                return false;
            break;

        case TOKEN_Key_return:
            int index = 0;
            tableItem *item = symGetItem(table, tokenID, 0);
            if(item == NULL){
                fprintf(stderr,"99"); //TODO - add error code
                freeToken(cToken);
                return false;
            }
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
            //call other parser
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

    if((cToken = nextToken()) == NULL)
        return false;
    if(cmpTokType(cToken, TOKEN_ID)){
        item = symGetItem(table, cToken->content.str, scope);
        if(item->type == FUNC_ID){
            int index = symstackCount(symStack);
            if(index > item->returnAmount){
                fprintf(stderr,"5"); //TODO - add error code
                freeToken(cToken);
                return false;
            }
            symstackItem *tmp;
            while(!symstackIsEmpty(symStack)){
                symstackPop(symStack, tmp);
                if(tmp->item->type != item->returnTypes[--index]){
                    fprintf(stderr,"5"); //TODO - add error code
                    freeToken(cToken);
                    return false;
                }
            }
        }
        else{
            //call other parser
        }
    }
    else if (cmpTokType(cToken, TOKEN_Num) || cmpTokType(cToken, TOKEN_Int) || cmpTokType(cToken, TOKEN_String)){
        //call other parser
    }
    else{
        fprintf(stderr,"5"); //TODO - add error code
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
        fprintf(stderr,"2"); //TODO - add error code
        freeToken(cToken);
        return false;
    }
    tableItem *item = symGetItem(table, cToken->content.str, scope);
    if(item->type == FUNC_ID){
        fprintf(stderr,"7"); //TODO - add error code
        freeToken(cToken);
        return false;
    }

    symstackPush(symStack, item);

    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_Comma)){
        returnToken = cToken;
        return true;
    }

    if(!pID())
        return false;
    return true;
}

// ---------------
int main(){
    if((table = malloc(sizeof(symTable))) == NULL)
        return 1;
    if((argStack = malloc(sizeof(stack))) == NULL){
        free(table);
        return 1;
    }
    if((symStack = malloc(sizeof(stack))) == NULL){
        free(argStack);
        free(table);
        return 1;
    }
    symInit(table);
    stackInit(argStack);
    symstackInit(symStack);
    bool isCorrect = pBody();
    int dump;
    tableItem *ptrDump;
    symDeleteAll(table);
    while(!stackIsEmpty(argStack)){
        stackPop(argStack, &dump);
    }
    while(!symstackIsEmpty(symStack)){
        symstackPop(symStack, ptrDump);
    }
    free(table);
    free(argStack);
    free(symStack);
    if (!isCorrect)
        return 1;
    return 0;
}

// --End of file--