/*
 * File: generator.c
 * file for generating IFJcode21
 * Subject: IFJ
 * Authors: Jakub Julius Smykal, Ondrej Kovac
 * Year: 2021
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "generator.h"


contentInput defBuffer = {.str = NULL, .index = 0, .length = 1};
contentInput blockBuffer = {.str = NULL, .index = 0, .length = 1};
contentInput callBuffer = {.str = NULL, .index = 0, .length = 1};
contentInput postCallBuffer = {.str = NULL, .index = 0, .length = 1};

bool mallocBuffer( contentInput *buffer){
    if((buffer->str = malloc(buffer->length)) == NULL){
        return false;
    }
    buffer->str[0] = '\0';
    return true;
}

bool mallocBuffers(){
    if(!mallocBuffer(&defBuffer))
        return false;
    if(!mallocBuffer(&blockBuffer))
        return false;
    if(!mallocBuffer(&callBuffer))
        return false;
    if(!mallocBuffer(&postCallBuffer))
        return false;
    return true;
}

void freeBuffer( contentInput *buffer){
    if(buffer->str != NULL){
        free(buffer->str);
        buffer->str = NULL;
    }
}

void freeBuffers(){
    freeBuffer(&defBuffer);
    freeBuffer(&blockBuffer);
    freeBuffer(&callBuffer);
    freeBuffer(&postCallBuffer);
}

bool insertString( contentInput *buffer, char *string){
    int strLen = strlen(string);
    if((buffer->str = realloc(buffer->str, buffer->length + strLen)) == NULL){
        return false;
    }
    strcat(buffer->str, string);
    buffer->length += strLen;
    buffer->index = buffer->length - 1;
    buffer->str[buffer->index] = '\0';
    return true;
}

bool genToInteger(){
    if(!insertString(&defBuffer,"\
    \nLABEL tointeger\
    \nCREATEFRAME\
    \nDEFVAR LF@retval1\
    \nMOVE LF@retval1 nil@nil\
    \nJUMPIFEQ tointeger_end LF@param1 nil@nil\
    \nFLOAT2INT LF@retval1 LF@param1\
    \nLABEL tointeger_end\
    \nPOPFRAME\
    \nRETURN"))
        return false;
    return true;
}

bool genSubstr(){
    if(!insertString(&defBuffer, "\
    \nLABEL substr\
    \nCREATEFRAME\
    \nDEFVAR LF@retval1\
    \nMOVE LF@retval1 string@\
    \nFLOAT2INT LF@param2 LF@param2\
    \nFLOAT2INT LF@param3 LF@param3\
    \nDEFVAR TF@cond\
    \nLT TF@cond LF@param2 int@1\
    \nJUMPIFEQ substr_end TF@cond bool@true\
    \nGT TF@cond LF@param2 LF@param3\
    \nJUMPIFEQ substr_end TF@cond bool@true\
    \nDEFVAR TF@strlen\
    \nSTRLEN TF@strlen LF@param1\
    \nGT TF@cond LF@param3 TF@strlen\
    \nJUMPIFEQ substr_end TF@cond bool@true\
    \nDEFVAR TF@counter\
    \nDEFVAR TF@tmp\
    \nMOVE TF@tmp string@\
    \nMOVE TF@counter int@0\
    \nSUB LF@param2 LF@param2 int@1\
    \nLABEL substr_loop\
    \nGETCHAR TF@tmp LF@param1 LF@param2\
    \nCONCAT LF@retval1 LF@retval1 TF@tmp\
    \nADD TF@counter TF@counter int@1\
    \nADD LF@param2 LF@param2 int@1\
    \nJUMPIFNEQ substr_loop LF@param2 lf@param3\
    \nLABEL substr_end\
    \nPOPFRAME\
    \nRETURN"))
        return false;
    return true;
}

bool genOrd(){
    if(!insertString(&defBuffer,"\
    \nLABEL ord\
    \nCREATEFRAME\
    \nDEFVAR LF@retval1\
    \nMOVE LF@retval1 nil@nil\
    \nDEFVAR TF@cond\
    \nLT TF@cond LF@param2 int@1\
    \nJUMPIFEQ ord_end TF@cond bool@true\
    \nDEFVAR TF@strlen\
    \nSTRLEN TF@strlen LF@param1\
    \nGT TF@cond LF@param2 TF@strlen\
    \nJUMPIFEQ ord_end TF@cond bool@true\
    \nSUB LF@param2 LF@param2 int@1\
    \nSTRI2INT LF@retval1 LF@param1 LF@param2\
    \nLABEL ord_end\
    \nPOPFRAME\
    \nRETURN"))
        return false;
    return true;
}

bool genChr(){
    if(!insertString(&defBuffer, "\
    LABEL chr\
    \nCREATEFRAME\
    \nDEFVAR LF@retval1\
    \nMOVE LF@retval1 nil@nil\
    \nDEFVAR TF@cond\
    \nLT TF@cond LF@param1 int@0\
    \nJUMPIFEQ chr_end TF@cond bool@true\
    \nGT TF@cond LF@param1 int@255\
    \nJUMPIFEQ chr_end TF@cond bool@true\
    \nINT2CHAR LF@retval1 LF@param1\
    \nLABEL chr_end\
    \nPOPFRAME\
    \nRETURN"))
        return false;
    return true;
}

bool genCallArg( contentInput *buffer, int counter, token *cToken){
    GEN_CODE(buffer, "\nDEFVAR TF@param")
    char tmp[25];
    float ftmp;
    sprintf(tmp, "%d", counter);
    GEN_CODE(buffer, tmp)
    GEN_CODE(buffer, "\nMOVE TF@param")
    GEN_CODE(buffer, tmp)
    switch(cToken->type){
        case TOKEN_String:
            GEN_CODE(buffer, " string@")
            GEN_CODE(buffer, cToken->content)
            break;

        case TOKEN_Int:
            GEN_CODE(buffer, " int@")
            GEN_CODE(buffer, cToken->content)
            break;

        case TOKEN_Num:
            GEN_CODE(buffer, " float@")
            ftmp = atof(cToken->content);
            snprintf(tmp, 25,"%a", ftmp);
            GEN_CODE(buffer, tmp)
            break;

        case TOKEN_Key_nil:
            GEN_CODE(buffer, " nil@nil")
            break;

        case TOKEN_ID:
            GEN_CODE(buffer, " LF@")
            GEN_CODE(buffer, cToken->content)
            break;
        
        default: return false;
    }
    return true;
}

bool genWrite( contentInput *buffer, token *cToken){
    char tmp[25];
    float ftmp;
    switch(cToken->type){
        case TOKEN_ID:
            GEN_CODE(buffer,"\nWRITE TF@")
            GEN_CODE(buffer, cToken->content);
            break;

        case TOKEN_Int:
            GEN_CODE(buffer,"\nWRITE int@")
            GEN_CODE(buffer, cToken->content);
            break;

        case TOKEN_Num:
            GEN_CODE(buffer,"\nWRITE float@")
            ftmp = atof(cToken->content);
            snprintf(tmp, 25,"%a", ftmp);
            GEN_CODE(buffer, tmp);
            break;
        
        case TOKEN_Key_nil:
            GEN_CODE(buffer,"\nWRITE nil@nil")
            break;
            
        case TOKEN_String:
            GEN_CODE(buffer,"\nWRITE string@")
            GEN_CODE(buffer, cToken->content);
            break;

        default: return false;
    }
    return true;
}

bool genVar( contentInput *buffer, tableItem *item){
    if(item->scope == 1){
        GEN_CODE(buffer, item->name)
    }
    else{
        GEN_CODE(buffer, item->name)
        GEN_CODE(buffer, "%")
        int blockIndex;
        if(item->scope == scope){
            stackTop(blockStack, &blockIndex);
        }
        else{
            int cScope = scope;
            int cIndex;
            stack storeStack;
            stackInit(&storeStack);
            while(cScope != item->scope){
                stackPop(blockStack, &cIndex);
                stackPush(&storeStack, cIndex);
                cScope--;
            }
            stackTop(blockStack, &blockIndex);
            while(!stackIsEmpty(&storeStack)){
                stackPop(&storeStack, &cIndex);
                stackPush(blockStack, cIndex);
            }
        }
        char tmp[10];
        sprintf(tmp, "%d", blockIndex);
        GEN_CODE(buffer, tmp)
    }
}

// --End of file-