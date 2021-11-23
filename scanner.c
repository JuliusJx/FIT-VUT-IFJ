/*
 * File: scanner.c
 * c file for lexical analysis
 * Subject: IFJ
 * Authors: Jakub Julius Smykal, Ondrej Kovac, Martin Talajka
 * Year: 2021
 */

#include <stdio.h>
#include "scanner.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

//Function inserts input c to data structure cInput
//Function doubles the size of cInput if array is full (Memory doesn't have to be reallocated as often, but wastes more space)
//Function returns true if successful, otherwise returns false
void InsertChar(contentInput *cInput, char c, bool *MemErr){
    while(cInput->index + 1 >= cInput->length){
        cInput->length *= 2;
        if ((cInput->str = realloc(cInput->str, cInput->length)) == NULL)
            *MemErr = true;
    }
    cInput->str[cInput->index] = c;
    cInput->index += 1;
}

//Function checks if string is keyword
//Function returns number asociated with given keyword or number asociated with common id (not keyword)
int isKeyWord(contentInput *cInput){
    if(!strcmp(cInput->str,"do"))
        return TOKEN_Key_do;
    else if(!strcmp(cInput->str,"else"))
        return TOKEN_Key_else;
    else if(!strcmp(cInput->str,"end"))
        return TOKEN_Key_end;
    else if(!strcmp(cInput->str,"function"))
        return TOKEN_Key_function;
    else if(!strcmp(cInput->str,"global"))
        return TOKEN_Key_global;
    else if(!strcmp(cInput->str,"if"))
        return TOKEN_Key_if;
    else if(!strcmp(cInput->str,"integer"))
        return TOKEN_Key_integer;
    else if(!strcmp(cInput->str,"local"))
        return TOKEN_Key_local;
    else if(!strcmp(cInput->str,"nil"))
        return TOKEN_Key_nil;
    else if(!strcmp(cInput->str,"number"))
        return TOKEN_Key_number;
    else if(!strcmp(cInput->str,"require"))
        return TOKEN_Key_require;
    else if(!strcmp(cInput->str,"return"))
        return TOKEN_Key_return;
    else if(!strcmp(cInput->str,"string"))
        return TOKEN_Key_string;
    else if(!strcmp(cInput->str,"then"))
        return TOKEN_Key_then;
    else if(!strcmp(cInput->str,"while"))
        return TOKEN_Key_while;
    else 
        return TOKEN_ID;
}

//Function reads input and returns pointer to token it created
//Function uses FSM to determine what type of token is being read
token *GetToken(){
    //Counter for line in input file
    static int line = 1;
    //Creates new token and allocates memory
    token *NewToken;
    if((NewToken = malloc(sizeof(token))) == NULL)
        return NULL;
    //Initializes newInput with starting size of 8 (to avoid unnecessary reallocations with small arrays)
    contentInput newInput = {.index = 0, .length = 8};
    if((newInput.str = malloc(newInput.length)) == NULL){
        free(NewToken);
        return NULL;
    }
    if(newInput.index == 0)
        for(int i = 0; i < newInput.length; i++)
            newInput.str[i] = '\0';
    else
        newInput.str[0] = '\0';
    //Stores read character from stdin
    char c;
    bool done = false, error = false, MemError = false;
    //Stores current state of FSM
    unsigned short FSM_State = FSM_Start;
    //FSM
    while((!done) && (!error) && (!MemError)){
        //Reads character from stdin
        c = getc(stdin);
        switch (FSM_State){
            case FSM_Start:
                if(c == '"')
                    FSM_State = FSM_String;
                else if((c == '_') || ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'))){
                    InsertChar(&newInput, c, &MemError);
                    FSM_State = FSM_ID;
                }
                else if((c >= '0') && (c <= '9')){
                    InsertChar(&newInput, c, &MemError);
                    FSM_State = FSM_Int;
                }
                else if(c == '+'){
                    NewToken->token = TOKEN_Plus;
                    done = true;
                }
                else if(c == '-')
                    FSM_State = FSM_Minus;
                else if(c == '*'){
                    NewToken->token = TOKEN_Mul;
                    done = true;
                }
                else if(c == '/')
                    FSM_State = FSM_Div;
                else if(c == '#'){
                    NewToken->token = TOKEN_StrLen;
                    done = true;
                }
                else if(c == ':'){
                    NewToken->token = TOKEN_Define;
                    done = true;
                }
                else if(c == '('){
                    NewToken->token = TOKEN_LeftPar;
                    done = true;
                }
                else if(c == ')'){
                    NewToken->token = TOKEN_RightPar;
                    done = true;
                }
                else if(c == ','){
                    NewToken->token = TOKEN_Separator;
                    done = true;
                }
                else if(c == '.')
                    FSM_State = FSM_Concat;
                else if(c == '<')
                    FSM_State = FSM_Less;
                else if(c == '>')
                    FSM_State = FSM_Greater;
                else if(c == '=')
                    FSM_State = FSM_Assign;
                else if(c == '~')
                    FSM_State = FSM_NotEqual;
                else if(c == '\n')
                    (line)++;
                else if(c == EOF){
                    NewToken->token = TOKEN_EOF;
                    done = true;
                }
                break;
            case FSM_String:
                if(c == '"'){
                    if((NewToken->content.str = malloc(strlen(newInput.str) + 1)) == NULL)
                        error = true;
                    else{
                        NewToken->content.str = strcpy(NewToken->content.str, newInput.str);
                        NewToken->token = TOKEN_String;
                    }
                    done = true;
                }
                else if(c == '\\'){
                    InsertChar(&newInput, c, &MemError);
                    FSM_State = FSM_StrEsc;
                }
                else if(c == ' '){
                    InsertChar(&newInput, '\\', &MemError);
                    InsertChar(&newInput, '0', &MemError);
                    InsertChar(&newInput, '3', &MemError);
                    InsertChar(&newInput, '2', &MemError);
                }
                else if(c == '#'){
                    InsertChar(&newInput, '\\', &MemError);
                    InsertChar(&newInput, '0', &MemError);
                    InsertChar(&newInput, '3', &MemError);
                    InsertChar(&newInput, '5', &MemError);
                }
                else if((c >= 31) && (c <= 255))
                    InsertChar(&newInput, c, &MemError);
                break;
            case FSM_StrEsc:
                if(c == '"'){
                    InsertChar(&newInput, '0', &MemError);
                    InsertChar(&newInput, '3', &MemError);
                    InsertChar(&newInput, '4', &MemError);
                    FSM_State = FSM_String;
                }
                else if(c == '\\'){
                    InsertChar(&newInput, '0', &MemError);
                    InsertChar(&newInput, '9', &MemError);
                    InsertChar(&newInput, '2', &MemError);
                    FSM_State = FSM_String;
                }
                else if(c == 'n'){
                    InsertChar(&newInput, '0', &MemError);
                    InsertChar(&newInput, '1', &MemError);
                    InsertChar(&newInput, '0', &MemError);
                    FSM_State = FSM_String;
                }
                else if(c == 't'){
                    InsertChar(&newInput, '0', &MemError);
                    InsertChar(&newInput, '0', &MemError);
                    InsertChar(&newInput, '9', &MemError);
                    FSM_State = FSM_String;
                }
                else if((c == '0') || (c == '1')){
                    InsertChar(&newInput, c, &MemError);
                    FSM_State = FSM_StrEscA;
                }
                else if(c == '2'){
                    InsertChar(&newInput, c, &MemError);
                    FSM_State = FSM_StrEscB;
                }
                else
                    error = true;
                break;
            case FSM_StrEscA:
                if((c >= '0') && (c <= '9')){
                    InsertChar(&newInput, c, &MemError);
                    FSM_State = FSM_StrEscC;
                }
                else
                    error = true;
                break;
            case FSM_StrEscB:
                if((c >= '0') && (c <= '4')){
                    InsertChar(&newInput, c, &MemError);
                    FSM_State = FSM_StrEscC;
                }
                if(c == '5'){
                    InsertChar(&newInput, c, &MemError);
                    FSM_State = FSM_StrEscD;
                }
                else
                    error = true;
                break;
            case FSM_StrEscC:
                if((c >= '0') && (c <= '9')){
                    InsertChar(&newInput, c, &MemError);
                    FSM_State = FSM_String;
                }
                else
                    error = true;
                break;
            case FSM_StrEscD:
                if((c >= '0') && (c <= '5')){
                    InsertChar(&newInput, c, &MemError);
                    FSM_State = FSM_String;
                }
                else
                    error = true;
                break;
            case FSM_ID:
                if(((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || ((c >= '0') && (c <= '9')) || (c == '_')){
                    InsertChar(&newInput, c, &MemError);
                }
                else{
                    ungetc(c, stdin);
                    NewToken->token = isKeyWord(&newInput);
                    if(NewToken->token == TOKEN_ID){
                        if((NewToken->content.str = malloc((strlen(newInput.str)) + 1)) == NULL)
                            error = true;
                        else{
                            NewToken->content.str = strcpy(NewToken->content.str, newInput.str);
                        }
                    }
                    done = true;
                }
                break;
            case FSM_Int:
                if((c >= '0') && (c <= '9')){
                    InsertChar(&newInput, c, &MemError);
                }
                else if(c == '.'){
                    InsertChar(&newInput, c, &MemError);
                    FSM_State = FSM_NumDotA;
                }
                else if((c == 'e') || (c == 'E')){
                    InsertChar(&newInput, c, &MemError);
                    FSM_State = FSM_NumEA;
                }
                else{
                    ungetc(c, stdin);
                    NewToken->token = TOKEN_Int;
                    NewToken->content.i = atoi(newInput.str);
                    done = true;
                }
                break;
            case FSM_NumDotA:
                if((c >= '0') && (c <= '9')){
                    InsertChar(&newInput, c, &MemError);
                    FSM_State = FSM_NumDot;
                }
                else
                    error = true;
                break;
            case FSM_NumDot:
                if((c == 'e') || (c == 'E')){
                    InsertChar(&newInput, c, &MemError);
                    FSM_State = FSM_NumEA;
                }
                else if((c >= '0') && (c <= '9')){
                    InsertChar(&newInput, c, &MemError);
                }
                else{
                    ungetc(c, stdin);
                    NewToken->token = TOKEN_Num;
                    NewToken->content.f = strtof(newInput.str, '\0');
                    done = true;
                }
                break;
            case FSM_NumEA:
                if ((c == '+') || (c == '-')){
                    InsertChar(&newInput, c, &MemError);
                    FSM_State = FSM_NumEB;
                }
                else if((c >= '0') && (c <= '9')){
                    InsertChar(&newInput, c, &MemError);
                    FSM_State = FSM_NumE;
                }
                else
                    error = true;
                break;
            case FSM_NumEB:
                if((c >= '0') && (c <= '9')){
                    InsertChar(&newInput, c, &MemError);
                    FSM_State = FSM_NumE;
                }
                else
                    error = true;
                break;
            case FSM_NumE:
                if((c >= '0') && (c <= '9')){
                    InsertChar(&newInput, c, &MemError);
                }
                else{
                    ungetc(c, stdin);
                    NewToken->token = TOKEN_Num;
                    NewToken->content.f = strtof(newInput.str, '\0');
                    done = true;
                }
                break;
            case FSM_Minus:
                if(c == '-')
                    FSM_State = FSM_OneLineC;
                else{
                    ungetc(c, stdin);
                    NewToken->token = TOKEN_Minus;
                    done = true;
                }
                break;
            case FSM_Div:
                if(c == '/')
                    NewToken->token = TOKEN_DivInt;
                else{
                    ungetc(c, stdin);
                    NewToken->token = TOKEN_Div;
                }
                done = true;
                break;
            case FSM_Concat:
                if(c == '.'){
                    NewToken->token = TOKEN_Concat;
                    done = true;
                }
                else
                    error = true;
                break;
            case FSM_Less:
                if(c == '=')
                    NewToken->token = TOKEN_LessEQ;
                else{
                    ungetc(c, stdin);
                    NewToken->token = TOKEN_Less;
                }
                done = true;
                break;
            case FSM_Greater:
                if(c == '=')
                    NewToken->token = TOKEN_GreatEQ;
                else{
                    ungetc(c, stdin);
                    NewToken->token = TOKEN_Greater;
                }
                done = true;
                break;
            case FSM_Assign:
                if(c == '=')
                    NewToken->token = TOKEN_EQ;
                else{
                    ungetc(c, stdin);
                    NewToken->token = TOKEN_Assign;
                }
                done = true;
                break;
            case FSM_NotEqual:
                if(c == '='){
                    NewToken->token = TOKEN_NotEQ;
                    done = true;
                }
                else
                    error = true;
            case FSM_OneLineC:
                if (c == '[')
                    FSM_State = FSM_OneLineOpenPar;
                else if((c == '\n') || (c == EOF)){
                    ungetc(c, stdin);
                    FSM_State = FSM_Start;
                }
                else
                    FSM_State = FSM_OneLineCA;
                break;
            case FSM_OneLineCA:
                if((c == '\n') || (c == EOF)){
                    ungetc(c, stdin);
                    FSM_State = FSM_Start;
                }
                break;
            case FSM_OneLineOpenPar:
                if(c == '[')
                    FSM_State = FSM_MultiLineC;
                else if((c == '\n') || (c == EOF)){
                    ungetc(c, stdin);
                    FSM_State = FSM_Start;
                }
                else
                    FSM_State = FSM_OneLineCA;
                break;
            case FSM_MultiLineC:
                if(c == ']')
                    FSM_State = FSM_MultilineClosePar;
                else if(c == '\n')
                    (line)++;
                else if(c == EOF)
                    error = true;
                break;
            case FSM_MultilineClosePar:
                if(c == ']')
                    FSM_State = FSM_Start;
                else if(c == EOF)
                    error = true;
                else if(c == '\n')
                    (line)++;
                else
                    FSM_State = FSM_MultiLineC;
                break;
            default:
                break;
        }
    }
    if(error){
        if((NewToken->content.str = malloc(strlen(newInput.str) + 1)) == NULL)
            MemError = true;
        else
            NewToken->content.str = strcpy(NewToken->content.str, newInput.str);
        ungetc(c, stdin);
        NewToken->token = TOKEN_Err;
    }
    free(newInput.str);
    if(MemError){
        free(NewToken);
        return NULL;
    }
    NewToken->line = line;
    return NewToken;
}

// --End of file--