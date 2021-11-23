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
char *tokenID = NULL; //maybe rename to functionID
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
    if(!pCallArgs())
        return false;
    
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
        returnToken = cToken;
        return true;
    }
    else{
        tableItem *item = symGetItem(table, cToken->content.str, scope);
        if(item == NULL){
            fprintf(stderr,"3"); //TODO - add error code
            freeToken(cToken);
            return false;
        }
        if(item->scope <= scope){
            fprintf(stderr,"3"); //TODO - add error code
            freeToken(cToken);
            return false;
        }
        int type;
        if(!stackPop(argStack, &type)){
            fprintf(stderr,"99"); //TODO - add error code
            freeToken(cToken);
            return false;
        }
        if(item->type != type){
            fprintf(stderr,"4"); //TODO - add error code
            freeToken(cToken);
            return false;
        }
        if(!pID(true))
            return false;
        return true;
    }
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
    if(!pID(true)) //TODO tu ma byt false
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

bool pID(bool semCheck){
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
    if(!pID(false)) //tu uz asi ma byt false
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
    symInit(table);
    stackInit(argStack);
    bool isCorrect = pBody();
    int dump;
    symDeleteAll(table);
    while(!stackIsEmpty(argStack)){
        stackPop(argStack, &dump);
    }
    free(table);
    free(argStack);
    if (!isCorrect)
        return 1;
    return 0;
}

// --End of file--