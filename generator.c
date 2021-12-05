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
    \nDEFVAR LF@retval%1\
    \nMOVE LF@retval%1 nil@nil\
    \nJUMPIFEQ tointeger_end LF@param%1 nil@nil\
    \nFLOAT2INT LF@retval%1 LF@param%1\
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
    \nDEFVAR LF@retval%1\
    \nMOVE LF@retval%1 string@\
    \nFLOAT2INT LF@param%2 LF@param%2\
    \nFLOAT2INT LF@param%3 LF@param%3\
    \nDEFVAR TF@cond\
    \nLT TF@cond LF@param%2 int@1\
    \nJUMPIFEQ substr_end TF@cond bool@true\
    \nGT TF@cond LF@param%2 LF@param%3\
    \nJUMPIFEQ substr_end TF@cond bool@true\
    \nDEFVAR TF@strlen\
    \nSTRLEN TF@strlen LF@param%1\
    \nGT TF@cond LF@param%3 TF@strlen\
    \nJUMPIFEQ substr_end TF@cond bool@true\
    \nDEFVAR TF@counter\
    \nDEFVAR TF@tmp\
    \nMOVE TF@tmp string@\
    \nMOVE TF@counter int@0\
    \nSUB LF@param%2 LF@param%2 int@1\
    \nLABEL substr_loop\
    \nGETCHAR TF@tmp LF@param%1 LF@param%2\
    \nCONCAT LF@retval%1 LF@retval%1 TF@tmp\
    \nADD TF@counter TF@counter int@1\
    \nADD LF@param%2 LF@param%2 int@1\
    \nJUMPIFNEQ substr_loop LF@param%2 lf@param%3\
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
    \nDEFVAR LF@retval%1\
    \nMOVE LF@retval%1 nil@nil\
    \nDEFVAR TF@cond\
    \nLT TF@cond LF@param%2 int@1\
    \nJUMPIFEQ ord_end TF@cond bool@true\
    \nDEFVAR TF@strlen\
    \nSTRLEN TF@strlen LF@param%1\
    \nGT TF@cond LF@param%2 TF@strlen\
    \nJUMPIFEQ ord_end TF@cond bool@true\
    \nSUB LF@param%2 LF@param%2 int@1\
    \nSTRI2INT LF@retval%1 LF@param%1 LF@param%2\
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
    \nDEFVAR LF@retval%1\
    \nMOVE LF@retval%1 nil@nil\
    \nDEFVAR TF@cond\
    \nLT TF@cond LF@param%1 int@0\
    \nJUMPIFEQ chr_end TF@cond bool@true\
    \nGT TF@cond LF@param%1 int@255\
    \nJUMPIFEQ chr_end TF@cond bool@true\
    \nINT2CHAR LF@retval%1 LF@param%1\
    \nLABEL chr_end\
    \nPOPFRAME\
    \nRETURN"))
        return false;
    return true;
}

bool genCallArgLit( contentInput *buffer, int counter, token *cToken){
    GEN_CODE(buffer, "\nDEFVAR TF@param%")
    char tmp[25];
    float ftmp;
    sprintf(tmp, "%d", counter);
    GEN_CODE(buffer, tmp)
    GEN_CODE(buffer, "\nMOVE TF@param%")
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
        
        default: return false;
    }
    return true;
}

bool genWriteLit( contentInput *buffer, token *cToken){
    char tmp[25];
    float ftmp;
    switch(cToken->type){
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

bool genCallArgID( contentInput *buffer, int counter, tableItem *item){
    GEN_CODE(buffer, "\nDEFVAR TF@param%")
    char tmp[25];
    sprintf(tmp, "%d", counter);
    GEN_CODE(buffer, tmp)
    GEN_CODE(buffer, "\nMOVE TF@param%")
    GEN_CODE(buffer, tmp)
    GEN_CODE(buffer, " TF@")
    if(!genVar(buffer, item))
        return false;
    return true;
}

bool genWriteID( contentInput *buffer, tableItem *item){
    GEN_CODE(buffer, "\nWRITE TF@")
    if(!genVar(buffer, item))
        return false;
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
                if(!stackPop(blockStack, &cIndex))
                    return false;
                if(!stackPush(&storeStack, cIndex))
                    return false;
                cScope--;
            }
            if(!stackTop(blockStack, &blockIndex))
                return false;
            while(!stackIsEmpty(&storeStack)){
                if(!stackPop(&storeStack, &cIndex))
                    return false;
                if(!stackPush(blockStack, cIndex))
                    return false;
            }
        }
        char tmp[10];
        sprintf(tmp, "%d", blockIndex);
        GEN_CODE(buffer, tmp)
    }
    return true;
}

// --End of file-