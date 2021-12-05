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
int blockCounter = 0;
stack *blockStack;

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
        errPrint(1, tmp, "bad_token");
        freeToken(tmp);
        return NULL;
    }
    return tmp;
}

void freeToken( token *token){
    if((token->type == TOKEN_ID) || (token->type == TOKEN_String) || (token->type == TOKEN_Err)){
        if(tokenID != token->content)
            free(token->content);
    }
    free(token);
}

bool insertBuiltIn(){
    tableItem *item;
    if(!symInsert(table, "reads", FUNC_ID, true, 0)){
        fprintf(stderr, "Internal Compiler Error");
        errCode = 99;
        return false;
    }
    item = symGetItem(table, "reads", 0);
    if(!symAddReturn(item, TYPE_STR)){
        fprintf(stderr, "Internal Compiler Error");
        errCode = 99;
        return false;
    }
    
    if(!symInsert(table, "readi", FUNC_ID, true, 0)){
        fprintf(stderr, "Internal Compiler Error");
        errCode = 99;
        return false;
    }
    item = symGetItem(table, "readi", 0);
    if(!symAddReturn(item, TYPE_INT)){
        fprintf(stderr, "Internal Compiler Error");
        errCode = 99;
        return false;
    }
    
    if(!symInsert(table, "readn", FUNC_ID, true, 0)){
        fprintf(stderr, "Internal Compiler Error");
        errCode = 99;
        return false;
    }
    item = symGetItem(table, "readn", 0);
    if(!symAddReturn(item, TYPE_NUM)){
        fprintf(stderr, "Internal Compiler Error");
        errCode = 99; 
        return false;
    }
    
    if(!symInsert(table, "write", FUNC_ID, true, 0)){
        fprintf(stderr, "Internal Compiler Error");
        errCode = 99; 
        return false;
    }

    if(!symInsert(table, "tointeger", FUNC_ID, true, 0)){
        fprintf(stderr, "Internal Compiler Error");
        errCode = 99; 
        return false;
    }
    item = symGetItem(table, "tointeger", 0);
    if(!symAddParam(item, TYPE_NUM)){
        fprintf(stderr, "Internal Compiler Error");
        errCode = 99; 
        return false;
    }
    if(!symAddReturn(item, TYPE_INT)){
        fprintf(stderr, "Internal Compiler Error");
        errCode = 99; 
        return false;
    }

    if(!symInsert(table, "substr", FUNC_ID, true, 0)){
        fprintf(stderr, "Internal Compiler Error");
        errCode = 99; 
        return false;
    }
    item = symGetItem(table, "substr", 0);
    if(!symAddParam(item, TYPE_STR)){
        fprintf(stderr, "Internal Compiler Error");
        errCode = 99; 
        return false;
    }
    if(!symAddParam(item, TYPE_NUM)){
        fprintf(stderr, "Internal Compiler Error");
        errCode = 99; 
        return false;
    }
    if(!symAddParam(item, TYPE_NUM)){
        fprintf(stderr, "Internal Compiler Error");
        errCode = 99; 
        return false;
    }
    if(!symAddReturn(item, TYPE_STR)){
        fprintf(stderr, "Internal Compiler Error");
        errCode = 99; 
        return false;
    }
    
    if(!symInsert(table, "ord", FUNC_ID, true, 0)){
        fprintf(stderr, "Internal Compiler Error");
        errCode = 99; 
        return false;
    }
    item = symGetItem(table, "ord", 0);
    if(!symAddParam(item, TYPE_STR)){
        fprintf(stderr, "Internal Compiler Error");
        errCode = 99; 
        return false;
    }
    if(!symAddParam(item, TYPE_INT)){
        fprintf(stderr, "Internal Compiler Error");
        errCode = 99; 
        return false;
    }
    if(!symAddReturn(item, TYPE_INT)){
        fprintf(stderr, "Internal Compiler Error");
        errCode = 99; 
        return false;
    }
    
    if(!symInsert(table, "chr", FUNC_ID, true, 0)){
        fprintf(stderr, "Internal Compiler Error");
        errCode = 99; 
        return false;
    }
    item = symGetItem(table, "chr", 0);
    if(!symAddParam(item, TYPE_INT)){
        fprintf(stderr, "Internal Compiler Error");
        errCode = 99;
        return false;
    }
    if(!symAddReturn(item, TYPE_STR)){
        fprintf(stderr, "Internal Compiler Error");
        errCode = 99; 
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
    ERR_CHECK(!cmpTokType(cToken, TOKEN_Key_require),2,"exp_require") //expected require

    freeToken(cToken);
    if((cToken = nextToken()) == NULL){
        return false;
    }
 
    if(cmpTokType(cToken, TOKEN_String)){
        ERR_CHECK(strcmp(cToken->content, "ifj21"),7,"exp_ifj21") //missing ifj21
    }
    else{
        ERR_CHECK(true,2,"exp_string")//missing string after require
    }
    //###### CODEGEN ######
    GEN_CODE(&startBuffer,".IFJcode21\n")
    //this sounds better than free(null) lol
    tokenID = cToken->content;
    freeToken(cToken);
    //scope 0 represents scope, where all functions are defined
    scope = 0;
    if(!pBody()){
        return false;
    }
    
    if((cToken = nextToken()) == NULL){
        return false;
    }
    ERR_CHECK(!cmpTokType(cToken, TOKEN_EOF),2,"exp_eof")//file should've ended

    //###### CODEGEN ######
    GEN_CODE(&callBuffer, "\nJUMP %END")
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

            ERR_CHECK(!cmpTokType(cToken, TOKEN_ID),2,"exp_id") //expected ID

            ERR_CHECK(symGetItem(table, cToken->content, 0) != NULL,3,"id_defined") //ID is already defined

            free(tokenID);
            tokenID = cToken->content;

            ERR_CHECK(!symInsert(table, cToken->content, FUNC_ID, false, 0),99,"internal")

            freeToken(cToken);
            if((cToken = nextToken()) == NULL){
                return false;
            }

            ERR_CHECK(!cmpTokType(cToken, TOKEN_Colon),2,"exp_colon")
            
            freeToken(cToken);
            if((cToken = nextToken()) == NULL){
                return false;
            }

            ERR_CHECK(!cmpTokType(cToken, TOKEN_Key_function),2,"exp_function")// expected keyword function
            
            freeToken(cToken);
            if((cToken = nextToken()) == NULL){
                return false;
            }

            ERR_CHECK(!cmpTokType(cToken, TOKEN_LeftPar),2,"exp_leftpar")
            
            freeToken(cToken);
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if((cmpTokType(cToken, TOKEN_Key_string)) || (cmpTokType(cToken, TOKEN_Key_integer)) || (cmpTokType(cToken, TOKEN_Key_number)) || (cmpTokType(cToken, TOKEN_Key_nil))){
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

            ERR_CHECK(!cmpTokType(cToken, TOKEN_RightPar),2,"exp_rightpar")
            
            freeToken(cToken);
            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(cmpTokType(cToken, TOKEN_Colon)){
                freeToken(cToken);
                if(!pReturns(false))
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

            ERR_CHECK(!cmpTokType(cToken, TOKEN_ID),2,"exp_id")

            item = symGetItem(table, cToken->content, 0);
            if(item != NULL){
                ERR_CHECK(item->isInit,3,"func_init")//function already initialized 
                symToggleInit(table, cToken->content, 0);
            }
            else{
                symInsert(table, cToken->content, FUNC_ID, false, 0);
            }
            free(tokenID);
            tokenID = cToken->content;
            freeToken(cToken);
            
            //###### CODEGEN ######
            GEN_CODE(&defBuffer, "\n\nLABEL ")
            GEN_CODE(&defBuffer, tokenID)
            GEN_CODE(&defBuffer, "\nCREATEFRAME")

            if((cToken = nextToken()) == NULL){
                return false;
            }

            ERR_CHECK(!cmpTokType(cToken, TOKEN_LeftPar),2,"exp_leftpar")
 
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

            ERR_CHECK(!cmpTokType(cToken, TOKEN_RightPar),2,"exp_rightpar")

            item = symGetItem(table, tokenID, 0);
            freeToken(cToken);

            if((cToken = nextToken()) == NULL){
                return false;
            }
            if(cmpTokType(cToken, TOKEN_Colon)){
                freeToken(cToken);
                if(!pReturns(true))
                    return false;
            }
            else{

                ERR_CHECK(item->returnAmount > 0,3,"no_rets_in_def") //declaration of function has params but definition does not (missing colon?)

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

            ERR_CHECK(!cmpTokType(cToken, TOKEN_Key_end),2,"exp_end")

            //###### CODEGEN ######
            GEN_CODE(&defBuffer, "\nPOPFRAME")
            GEN_CODE(&defBuffer, "\nRETURN")

            symDeleteScope(table, scope);
            scope--;
            freeToken(cToken);
            if(!pBody()){
                return false;
            }
            break;

        case TOKEN_ID:
            
            GEN_CODE(&callBuffer, "\nCREATEFRAME")
            GEN_CODE(&callBuffer, "\nPUSHFRAME")
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

    ERR_CHECK(!cmpTokType(cToken, TOKEN_ID),2,"exp_id")

    tableItem *item = symGetItem(table, cToken->content, 0);

    ERR_CHECK(item == NULL,3,"undef_id") //use of undefined ID

    ERR_CHECK(item->type != FUNC_ID,3,"call_no_f_id") //only functions can be called (got variable)

    ERR_CHECK((item->returnAmount > 0) && (scope == 0),5,"ret_in_main")//functions with returns cannot be called outside of function definitions (no global variables)

    callFuncID = item;
    symToggleUsed(table, cToken->content, 0); //all functions are defined at scope 0
    int i = item->paramAmount - 1;
    while (i >= 0){
        stackPush(argStack, item->paramTypes[i--] - '0');
    }
    freeToken(cToken);

    if((cToken = nextToken()) == NULL){
        return false;
    }

    ERR_CHECK(!cmpTokType(cToken, TOKEN_LeftPar),2,"exp_leftpar")

    freeToken(cToken);

    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(cmpTokType(cToken, TOKEN_ID) || cmpTokType(cToken, TOKEN_String) || cmpTokType(cToken, TOKEN_Int) || cmpTokType(cToken, TOKEN_Num) || cmpTokType(cToken, TOKEN_Key_nil)){
        returnToken = cToken;
        //###### CODEGEN ######
        if(strcmp(callFuncID->name,"write")){
            if(scope == 0){
                GEN_CODE(&callBuffer,"\n\
                \nPUSHFRAME\
                \nCREATEFRAME")
            }
            else if(scope == 1){
                GEN_CODE(&defBuffer,"\n\
                \nPUSHFRAME\
                \nCREATEFRAME")
            }
            else{
                GEN_CODE(&blockBuffer,"\n\
                \nPUSHFRAME\
                \nCREATEFRAME")
            }

        }
        if(!pCallArgs())
            return false;
        //###### CODEGEN ######
        if(strcmp(callFuncID->name,"write") && strcmp(callFuncID->name,"reads") && strcmp(callFuncID->name,"readi") && strcmp(callFuncID->name,"readn")){
            if(scope == 1){
                GEN_CODE(&defBuffer,"\
                \nPUSHFRAME\
                \n\nCALL ")
                GEN_CODE(&defBuffer,callFuncID->name)
            }
            else{
                GEN_CODE(&defBuffer,"\
                \nPUSHFRAME\
                \n\nCALL ")
                GEN_CODE(&defBuffer,callFuncID->name)
            }//maybe add condition for scope == 0 TODO
        }
        
    }
    else{

        ERR_CHECK(!strcmp(item->name, "write"),5,"write_no_par") //write needs at least 1 param

        ERR_CHECK(item->paramAmount > 0,5,"not_enough_pars") //function expected more params

        //###### CODEGEN ######
        if(scope == 0){
            GEN_CODE(&callBuffer,"\n\nCALL ")
            GEN_CODE(&callBuffer,callFuncID->name)
        }
        else{
            if(strcmp(callFuncID->name,"reads") && strcmp(callFuncID->name,"readi") && strcmp(callFuncID->name,"readn")){ //if not read-s/i/n
                if(scope == 1){
                    GEN_CODE(&defBuffer,"\n\
                    \nPUSHFRAME\
                    \nCRATEFRAME\
                    \nPUSHFRAME\
                    \n\nCALL ")
                    GEN_CODE(&defBuffer,callFuncID->name)
                }
                else{
                    GEN_CODE(&blockBuffer,"\n\
                    \nPUSHFRAME\
                    \nCRATEFRAME\
                    \nPUSHFRAME\
                    \n\nCALL ")
                    GEN_CODE(&blockBuffer,callFuncID->name)
                }
            }
        }
        returnToken = cToken;
    }
    
    if((cToken = nextToken()) == NULL){
        return false;
    }

    ERR_CHECK(!cmpTokType(cToken, TOKEN_RightPar),2,"exp_rightpar")

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

        case TOKEN_Key_nil:
            type = TYPE_NIL;
            break;

        default:
            ERR_CHECK(true,2,"exp_type")//expected type key token
    }
    tableItem *item = symGetItem(table, tokenID, 0);

    ERR_CHECK(!symAddParam(item, type),99,"internal")

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

bool pReturns( bool definition){
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

        case TOKEN_Key_nil:
            type = TYPE_NIL;
            break;

        default:
            ERR_CHECK(true,2,"exp_type")
    }
    tableItem *item = symGetItem(table, tokenID, 0);
    if(item->isInit){

        ERR_CHECK(returnIndex >= item->returnAmount,3,"more_rets_in_def") //function has more returns in definitition than in declaration

        ERR_CHECK(type != item->returnTypes[returnIndex++] - '0',3,"diff_rets_in_def") //function has different returns in definitition than in declaration

    }
    else{
        returnIndex++;
        ERR_CHECK(!symAddReturn(item, type),99,"internal")
    }
    if(definition){
        //###### CODEGEN ######
        GEN_CODE(&defBuffer, "\n\nDEFVAR LF@retval%")
        char tmp[10];
        sprintf(tmp, "%d", returnIndex);
        GEN_CODE(&defBuffer, tmp)
        GEN_CODE(&defBuffer, "\nMOVE LF@retval%")
        GEN_CODE(&defBuffer, tmp)
        GEN_CODE(&defBuffer, " nil@nil")
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
    
    if(!pReturns(definition))
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

    ERR_CHECK(!cmpTokType(cToken, TOKEN_ID),2,"exp_id")

    ERR_CHECK(symGetItem(table, cToken->content, scope) != NULL,3,"redef_id") //redefinition of ID

    
    char *tmp = cToken->content; //maybe future error TODO something
    free(cToken); //freeToken() would also free content, which we need in tmp

    if((cToken = nextToken()) == NULL){
        return false;
    }

    ERR_CHECK(!cmpTokType(cToken, TOKEN_Colon),2,"exp_colon")
    
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

        case TOKEN_Key_nil:
            type = TYPE_NIL;
            break;

        default:
            ERR_CHECK(true,2,"exp_type")
    }

    if(item->isInit){
        ERR_CHECK(paramIndex >= item->paramAmount,3,"more_params_in_def") //function has more parameters in definitition than in declaration

        ERR_CHECK(type != item->returnTypes[paramIndex++] - '0',3,"diff_params_in_def") //function has different parameters in definitition than in declaration

    }
    else{
        paramIndex++;
        ERR_CHECK(!symAddParam(item, type),99,"internal")
    }
    symInsert(table, tmp, type, true, scope);
    freeToken(cToken);
    
    //###### CODEGEN ######
    GEN_CODE(&defBuffer, "\n\nDEFVAR TF@")
    GEN_CODE(&defBuffer, tmp)
    GEN_CODE(&defBuffer, "\nMOVE TF@")
    GEN_CODE(&defBuffer, tmp)
    char argTmp[10];
    sprintf(argTmp, "%d", paramIndex);
    GEN_CODE(&defBuffer, " LF@param%")
    GEN_CODE(&defBuffer, argTmp)

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
    static int paramCounter = 1;

    if((cToken = nextToken()) == NULL){
        return false;
    }
    int type;
    if(!cmpTokType(cToken, TOKEN_ID)){
        if(cmpTokType(cToken, TOKEN_String) || cmpTokType(cToken, TOKEN_Int) || cmpTokType(cToken, TOKEN_Num) || cmpTokType(cToken, TOKEN_Key_nil)){
            if(strcmp(callFuncID->name,"write")){

                ERR_CHECK(!stackPop(argStack, &type),5,"too_many_args") //more arguments in call than expected

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
                    case TYPE_NIL:
                        type = TOKEN_Key_nil;
                        break;
                    default: break;
                }
                if(cToken->type != TOKEN_Key_nil){
                    if(cToken->type != type){
                        ERR_CHECK((cToken->type != TYPE_INT) || (type != TYPE_NUM),5,"wrong_arg_type") //argument is the wrong type
                    }
                }
                //###### CODEGEN ######
                if(scope == 0){
                    ERR_CHECK(!genCallArgLit(&callBuffer, paramCounter, cToken),99,"internal")
                }
                else if(scope == 1){
                    ERR_CHECK(!genCallArgLit(&defBuffer, paramCounter, cToken),99,"internal")
                }
                else{
                    ERR_CHECK(!genCallArgLit(&blockBuffer, paramCounter, cToken),99,"internal")
                }
            }
            else{
                if(scope == 0){
                    ERR_CHECK(!genWriteLit(&callBuffer,cToken),99,"internal")
                }
                else if(scope == 1){
                    ERR_CHECK(!genWriteLit(&defBuffer,cToken),99,"internal")
                }
                else{
                    ERR_CHECK(!genWriteLit(&blockBuffer,cToken),99,"internal")
                }
            }
        }
        else{
            ERR_CHECK(true,2,"bad_arg") //argument is neither an ID nor a literal
        }
    }
    else{//if arg is ID
        tableItem *item = symGetItem(table, cToken->content, scope);

        ERR_CHECK(item == NULL,3,"undef_id") //used id is undefined

        if(!strcmp(callFuncID->name,"write")){
            if((item->type == TYPE_INT) || (item->type == TYPE_NUM) || (item->type == TYPE_STR) || (item->type == TYPE_NIL)){
                type = item->type;
            }
            else{
                ERR_CHECK(true,5,"function_as_arg")//tried to pass function as argument
            }
        }
        else{
            ERR_CHECK(!stackPop(argStack, &type),5,"too_many_args")
        }
        if(item->type != type){
            ERR_CHECK((item->type != TYPE_INT) || (type != TYPE_NUM),5,"wrong_arg_type")
        }
        //###### CODEGEN ######
        if(strcmp(callFuncID->name,"write")){
            if(scope == 1){
                ERR_CHECK(!genCallArgID(&defBuffer, paramCounter, item),99,"internal")
            }
            else{
                ERR_CHECK(!genCallArgID(&blockBuffer, paramCounter, item),99,"internal")
            }
        }
        else{
            if(scope == 1){
                ERR_CHECK(!genWriteID(&defBuffer, item),99,"internal")
            }
            else{
                ERR_CHECK(!genWriteID(&blockBuffer, item),99,"internal")
            }
        }
    }
    
    freeToken(cToken);

    //if next token is comma, recursive call
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_Comma)){
        returnToken = cToken;
        ERR_CHECK(!stackIsEmpty(argStack),5,"not_enough_args") //less arguments in call than expected
        paramCounter = 1;
        return true;
    }
    else
        freeToken(cToken);

    paramCounter++;
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
    
    int blockIndex;
    int index;
    switch(cToken->type){
        case TOKEN_Key_if:
            blockCounter++;
            stackPush(blockStack, blockCounter);
            freeToken(cToken);

            tableItem newItem = { 
                .type = T_BOOL,
                .isInit = true,
                .isUsed = false,
                .paramAmount = 0,
                .returnAmount = 0,
                .paramTypes = NULL,
                .returnTypes = NULL,
                .scope = scope,
                .next = NULL };
            char iftmp[10];
            stackTop(blockStack, &blockIndex);
            sprintf(iftmp, "%d", blockIndex);

            ERR_CHECK((newItem.name = malloc(8 * sizeof(char))) == NULL,99,"internal") //8 characters for ifcond% + terminal character

            strcpy(newItem.name, "ifcond%\0");

            ERR_CHECK((newItem.name = realloc(newItem.name, strlen(newItem.name) + strlen(iftmp) + 1)) == NULL,99,"internal")

            strcat(newItem.name, iftmp);
            //###### CODEGEN ######
            GEN_CODE(&defBuffer, "\n\nDEFVAR TF@")
            GEN_CODE(&defBuffer, newItem.name)
            
            /*symstackPush(symStack, &newItem);
            if(!pExpression(0))
                return false;*/
            if((cToken = nextToken()) == NULL){
                return false;
            }

            ERR_CHECK(!cmpTokType(cToken, TOKEN_Key_then),2,"exp_then")

            //###### CODEGEN ######
            GEN_CODE(&blockBuffer, "\nJUMPIFEQ ELSE%")
            GEN_CODE(&blockBuffer, iftmp);
            GEN_CODE(&blockBuffer, " TF@")
            GEN_CODE(&blockBuffer, newItem.name) //maybe freed somewhere, check it later
            GEN_CODE(&blockBuffer, " bool@false")

            scope++;
            if(!pStatement())
                return false;
            
            symDeleteScope(table, scope);
            scope--;
            freeToken(cToken);

            if((cToken = nextToken()) == NULL){
                return false;
            }
            ERR_CHECK(!cmpTokType(cToken, TOKEN_Key_else),2,"exp_else")
            blockCounter++;
            stackPush(blockStack, blockCounter);
            //###### CODEGEN ######
            GEN_CODE(&blockBuffer, "\nJUMP ENDIF%")
            GEN_CODE(&blockBuffer, iftmp);
            GEN_CODE(&blockBuffer, "\n\nLABEL ELSE%")
            GEN_CODE(&blockBuffer, iftmp);

            scope++;
            if(!pStatement())
                return false;
            symDeleteScope(table, scope);
            scope--;
            freeToken(cToken);

            if((cToken = nextToken()) == NULL){
                return false;
            }
            ERR_CHECK(!cmpTokType(cToken, TOKEN_Key_end),2,"exp_end")
            //###### CODEGEN ######
            GEN_CODE(&blockBuffer, "\n\nLABEL ENDIF%")
            GEN_CODE(&blockBuffer, iftmp);

            if(scope == 1){
                GEN_CODE(&defBuffer, blockBuffer.str)
                freeBuffer(&blockBuffer);
                mallocBuffer(&blockBuffer);
            }

            freeToken(cToken);
            if(!pStatement())
                return false;
            break;

        case TOKEN_Key_while:
            blockCounter++;
            stackPush(blockStack, blockCounter);
            freeToken(cToken);

            tableItem newWhileItem = { 
                .type = T_BOOL,
                .isInit = true,
                .isUsed = false,
                .paramAmount = 0,
                .returnAmount = 0,
                .paramTypes = NULL,
                .returnTypes = NULL,
                .scope = scope,
                .next = NULL };
            char whtmp[10];
            stackTop(blockStack, &blockIndex);
            sprintf(whtmp, "%d", blockIndex);

            ERR_CHECK((newWhileItem.name = malloc(8 * sizeof(char))) == NULL,99,"internal") //8 characters for whcond% + terminal character

            strcpy(newWhileItem.name, "whcond%\0");

            ERR_CHECK((newWhileItem.name = realloc(newWhileItem.name, strlen(newWhileItem.name) + strlen(whtmp) + 1)) == NULL,99,"internal")

            strcat(newWhileItem.name, whtmp);
            //###### CODEGEN ######
            GEN_CODE(&defBuffer, "\n\nDEFVAR TF@")
            GEN_CODE(&defBuffer, newWhileItem.name)
            GEN_CODE(&blockBuffer, "\n\nLABEL LOOP%")
            GEN_CODE(&blockBuffer, whtmp);
            /*symstackPush(symStack, &newWhileItem);
            if(!pExpression(0))
                return false;*/
            
            //###### CODEGEN ######
            GEN_CODE(&blockBuffer, "\nJUMPIFEQ LOOP_END%")
            GEN_CODE(&blockBuffer, whtmp);
            GEN_CODE(&blockBuffer, " TF@")
            GEN_CODE(&blockBuffer, newWhileItem.name) //maybe freed somewhere, check it later
            GEN_CODE(&blockBuffer, " bool@false")

            if((cToken = nextToken()) == NULL){
                return false;
            }
            ERR_CHECK(!cmpTokType(cToken, TOKEN_Key_do),2,"exp_do")

            scope++;
            if(!pStatement())
                return false;
            symDeleteScope(table, scope);
            scope--;
            freeToken(cToken);

            if((cToken = nextToken()) == NULL){
                return false;
            }
            ERR_CHECK(!cmpTokType(cToken, TOKEN_Key_end),2,"exp_end")

            //###### CODEGEN ######
            GEN_CODE(&blockBuffer, "\nJUMP LOOP%")
            GEN_CODE(&blockBuffer, whtmp);
            GEN_CODE(&blockBuffer, "\n\nLABEL LOOP_END%")
            GEN_CODE(&blockBuffer, whtmp);

            if(scope == 1){
                GEN_CODE(&defBuffer, blockBuffer.str)
                freeBuffer(&blockBuffer);
                mallocBuffer(&blockBuffer);
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
            ERR_CHECK(!cmpTokType(cToken, TOKEN_ID),2,"exp_id")
            //Add entry to symtable
            item = symGetItem(table, cToken->content, scope);
            if(item != NULL){
                ERR_CHECK((item->scope == scope) || (item->type == FUNC_ID),3,"redef_id")
            }
            char *tmp = cToken->content;
            free(cToken);

            if((cToken = nextToken()) == NULL){
                return false;
            }
            ERR_CHECK(!cmpTokType(cToken, TOKEN_Colon),2,"exp_colon")

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

                case TOKEN_Key_nil:
                    type = TYPE_NIL;
                    break;

                default:
                    ERR_CHECK(true,2,"exp_type")
            }
            ERR_CHECK(!symInsert(table, tmp, type, false, scope),99,"internal")
            freeToken(cToken);

            item = symGetItem(table, tmp, scope);
            //###### CODEGEN ######
            GEN_CODE(&defBuffer,"\n\nDEFVAR TF@")
            genVar(&defBuffer, item);
            symstackPush(symStack, item);
            if(!pInit())
                return false;
            symstackPop(symStack, &item);
            
            free(tmp);
            if(!pStatement())
                return false;
            break;

        case TOKEN_ID:
            
            item = symGetItem(table, cToken->content, scope);
            ERR_CHECK(item == NULL,3,"undef_id")
            if(item->type == FUNC_ID){
                returnToken = cToken;
                if(!pCall())
                    return false;
                if(strcmp(item->name,"write")){
                    if(scope == 1){
                        GEN_CODE(&defBuffer,"\nPOPFRAME")
                    }
                    else{
                        GEN_CODE(&blockBuffer,"\nPOPFRAME")
                    }
                }
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
                        item = symGetItem(table, cToken->content, scope);
                        ERR_CHECK(item == NULL,3,"undef_id")
                        if(item->type == FUNC_ID){
                            tableItem *tmp;
                            
                            index = symstackCount(symStack);
                            ERR_CHECK(index > item->returnAmount,5,"too_many_targets")//function has less returns than there are variables before assign
                            
                            while(!symstackIsEmpty(symStack)){
                                symstackPop(symStack, &tmp);
                                tmp->isInit = true;
                                ERR_CHECK(tmp->type != item->returnTypes[index - 1] - '0',5,"wrong_ret_type")//TODO check the number/int compatibility
                                if(strcmp(item->name,"reads") && strcmp(item->name,"readi") && strcmp(item->name,"readn")){
                                    //###### CODEGEN ######
                                    GEN_CODE(&postCallBuffer, "\nMOVE LF@")
                                    genVar(&postCallBuffer, tmp);
                                    GEN_CODE(&postCallBuffer, " TF@retval%");
                                    char buf[10]; //max number of return values is 999 999 999 lol
                                    sprintf(buf, "%d", index);
                                    GEN_CODE(&postCallBuffer, buf);
                                }
                                else{
                                    if(scope == 1){
                                        GEN_CODE(&defBuffer, "\nREAD TF@")
                                        genVar(&defBuffer, tmp);
                                        if(!strcmp(item->name,"reads")){
                                            GEN_CODE(&defBuffer, " string")
                                        }
                                        else if(!strcmp(item->name,"readi")){
                                            GEN_CODE(&defBuffer, " int")
                                        }
                                        else{
                                            GEN_CODE(&defBuffer, " float")
                                        }
                                    }
                                    else{
                                        GEN_CODE(&blockBuffer, "\nREAD TF@")
                                        genVar(&blockBuffer, tmp);
                                        if(!strcmp(item->name,"reads")){
                                            GEN_CODE(&blockBuffer, " string")
                                        }
                                        else if(!strcmp(item->name,"readi")){
                                            GEN_CODE(&blockBuffer, " int")
                                        }
                                        else{
                                            GEN_CODE(&blockBuffer, " float")
                                        }
                                    }
                                }
                                index--;
                            }
                            if(strcmp(item->name,"reads") && strcmp(item->name,"readi") && strcmp(item->name,"readn")){
                                //###### CODEGEN ######
                                GEN_CODE(&postCallBuffer, "\nPOPFRAME");
                            }
                            returnToken = cToken;
                            if(!pCall()){
                                return false;
                            }
                            //###### CODEGEN ######
                            if(scope == 1){
                                GEN_CODE(&defBuffer, postCallBuffer.str)
                            }
                            else{
                                GEN_CODE(&blockBuffer, postCallBuffer.str)
                            }
                            freeBuffer(&postCallBuffer);
                            mallocBuffer(&postCallBuffer);
                        }
                        else{
                            returnToken = cToken;
                            if(!pExpression(0))
                                return false;
                        }
                    }
                    else if (cmpTokType(cToken, TOKEN_Num) || cmpTokType(cToken, TOKEN_Int) || cmpTokType(cToken, TOKEN_String) || cmpTokType(cToken, TOKEN_StrLen) || cmpTokType(cToken, TOKEN_LeftPar) || cmpTokType(cToken, TOKEN_Key_nil)){
                        returnToken = cToken;
                        if(!pExpression(0))
                            return false;
                    }
                    else{
                        ERR_CHECK(true,5,"bad_assign")//trying to assign something that is not a funcID/ID/literal
                    }
                    
                }
                else{
                    ERR_CHECK(true,3,"missing_assign")
                }
            }
            //TODO - maybe leak here
            if(!pStatement())
                return false;
            break;

        case TOKEN_Key_return:
            index = 0;
            tableItem *item = symGetItem(table, tokenID, 0);
            ERR_CHECK(item == NULL,99,"internal")
            freeToken(cToken);
            while(index < item->returnAmount){
                tableItem newItem = { 
                    .type = item->returnTypes[index++]-'0',
                    .isInit = true,
                    .isUsed = false,
                    .paramAmount = 0,
                    .returnAmount = 0,
                    .paramTypes = NULL,
                    .returnTypes = NULL,
                    .scope = scope,
                    .next = NULL };
                char rettmp[10];
                sprintf(rettmp, "%d", index);
                ERR_CHECK((newItem.name = malloc(8 * sizeof(char))) == NULL,99,"internal")
                strcpy(newItem.name, "retval%\0");
                ERR_CHECK((newItem.name = realloc(newItem.name, strlen(newItem.name) + strlen(rettmp) + 1)) == NULL,99,"internal")
                strcat(newItem.name, rettmp);
                symstackPush(symStack, &newItem);
            }
            if(!pExpression(0))
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
    tableItem *item, *tmp;
    if(!cmpTokType(cToken, TOKEN_Assign)){
        symstackPop(symStack, &tmp);
        //###### CODEGEN ######
        GEN_CODE(&defBuffer, "\nMOVE TF@")
        genVar(&defBuffer, tmp);
        GEN_CODE(&defBuffer, " nil@nil")
        returnToken = cToken;
        return true;
    }
    freeToken(cToken);

    if((cToken = nextToken()) == NULL)
        return false;
    if(cmpTokType(cToken, TOKEN_ID)){
        item = symGetItem(table, cToken->content, scope);
        ERR_CHECK(item == NULL,3,"undef_id")
        if(item->type == FUNC_ID){
            int index = symstackCount(symStack);
            ERR_CHECK(index > item->returnAmount,5,"too_many_targets")
            symstackPop(symStack, &tmp);
            ERR_CHECK(tmp->type != item->returnTypes[--index] - '0',5,"wrong_ret_type")
            returnToken = cToken;
            if(strcmp(item->name,"reads") && strcmp(item->name,"readi") && strcmp(item->name,"readn")){
                //###### CODEGEN ######
                GEN_CODE(&postCallBuffer, "\nMOVE LF@")
                genVar(&postCallBuffer, tmp);
                GEN_CODE(&postCallBuffer, " TF@retval%1");
                GEN_CODE(&postCallBuffer, "\nPOPFRAME")
                if(!pCall())
                    return false;
            }
            else{
                //###### CODEGEN ######
                GEN_CODE(&defBuffer, "\nREAD TF@")
                genVar(&defBuffer, tmp);
                if(!strcmp(item->name,"reads")){
                    GEN_CODE(&defBuffer, " string")
                }
                else if(!strcmp(item->name,"readi")){
                    GEN_CODE(&defBuffer, " int")
                }
                else{
                    GEN_CODE(&defBuffer, " float")
                }
            }
            tmp->isInit = true;
            //###### CODEGEN ######
            GEN_CODE(&defBuffer, postCallBuffer.str)
            freeBuffer(&postCallBuffer);
            mallocBuffer(&postCallBuffer);
        }
        else{
            returnToken = cToken;
            symstackTop(symStack, &tmp);
            if(!pExpression(0))
                return false;
            tmp->isInit = true;
        }
    }
    else if (cmpTokType(cToken, TOKEN_Num) || cmpTokType(cToken, TOKEN_Int) || cmpTokType(cToken, TOKEN_String) || cmpTokType(cToken, TOKEN_StrLen) || cmpTokType(cToken, TOKEN_LeftPar)){
        returnToken = cToken;
        if(!pExpression(0))
            return false;
        symstackTop(symStack, &tmp);
        tmp->isInit = true;
    }
    else{
        ERR_CHECK(!cmpTokType(cToken, TOKEN_Key_nil),5,"bad_assign")//trying to init using something ither than funcID/ID/literal/expression/nil
    }
    return true;
}

bool pID(){
    token *cToken;
    
    if((cToken = nextToken()) == NULL){
        return false;
    }
    if(!cmpTokType(cToken, TOKEN_ID)){
        ERR_CHECK(!cmpTokType(cToken, TOKEN_ID),2,"exp_id")
    }
    tableItem *item = symGetItem(table, cToken->content, scope);
    ERR_CHECK(item->type == FUNC_ID,7,"func_as_target")

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
        errCode = 99;
        fprintf(stderr, "Internal Compiler Error");
        return errCode;
    }
    if((argStack = malloc(sizeof(stack))) == NULL){
        errCode = 99;
        free(table);
        fprintf(stderr, "Internal Compiler Error");
        return errCode;
    }
    if((symStack = malloc(sizeof(symstack))) == NULL){
        errCode = 99;
        free(argStack);
        free(table);
        fprintf(stderr, "Internal Compiler Error");
        return errCode;
    }
    if((blockStack = malloc(sizeof(stack))) == NULL){
        errCode = 99;
        free(argStack);
        free(table);
        free(symStack);
        fprintf(stderr, "Internal Compiler Error");
        return errCode;
    }
    symInit(table);
    stackInit(argStack);
    symstackInit(symStack);
    stackInit(blockStack);

    mallocBuffers();
    insertBuiltIn();
    pProgram();

    int dump;
    tableItem *ptrDump;
    symDeleteAll(table);
    //###### CODEGEN ######
    GEN_CODE(&defBuffer, "\n\nLABEL %END\n")
    while(!stackIsEmpty(argStack)){
        stackPop(argStack, &dump);
    }
    while(!symstackIsEmpty(symStack)){
        symstackPop(symStack, &ptrDump);
    }
    while(!stackIsEmpty(blockStack)){
        stackPop(blockStack, &dump);
    }
    free(table);
    free(argStack);
    free(symStack);
    free(blockStack);
    free(tokenID);
    
    if(!errCode)
        generateCode();
    
    return errCode;
}

// --End of file--