/*
 * File: scanner.c
 * c file for lexical analysis
 * Subject: IFJ
 * Authors: Jakub Julius Smykal
 * Year: 2021
 */

#include "stdio.h"
#include "scanner.h"
#include "string.h"
#include "stdbool.h"


int InsertChar(contentInput *cInput, char *c){
    int len = strlen(c);
    while(cInput->index + len > cInput->length){
        cInput->length *= 2;
        if ((cInput->str = realloc(cInput->str, cInput->length)) == NULL)
            return 1;
    }
    cInput->str = strcat(cInput->str, c);
    cInput->index += len;
    return 0;
}

int isKeyWord(contentInput *cInput){
    if(cInput->str == "do")
        return TOKEN_Key_do;
    else if(cInput->str == "else")
        return TOKEN_Key_else;
    else if(cInput->str == "end")
        return TOKEN_Key_end;
    else if(cInput->str == "function")
        return TOKEN_Key_function;
    else if(cInput->str == "global")
        return TOKEN_Key_global;
    else if(cInput->str == "if")
        return TOKEN_Key_if;
    else if(cInput->str == "integer")
        return TOKEN_Key_integer;
    else if(cInput->str == "local")
        return TOKEN_Key_local;
    else if(cInput->str == "nil")
        return TOKEN_Key_nil;
    else if(cInput->str == "number")
        return TOKEN_Key_number;
    else if(cInput->str == "require")
        return TOKEN_Key_require;
    else if(cInput->str == "return")
        return TOKEN_Key_return;
    else if(cInput->str == "string")
        return TOKEN_Key_string;
    else if(cInput->str == "then")
        return TOKEN_Key_then;
    else if(cInput->str == "while")
        return TOKEN_Key_while;
    else 
        return TOKEN_ID;
}

token *GetToken(){
    token *NewToken;
    if((NewToken = malloc(sizeof(token))) == NULL)
        return NULL;
    
    contentInput newInput = {.index = 0, .length = 8};
    if((newInput.str = malloc(newInput.length)) == NULL){
        free(NewToken);
        return NULL;
    }

    char c;
    bool done = false, error = false;
    unsigned line = 1;
    unsigned short FSM_State = FSM_Start;

    while((!done) && (!error)){
        c = getc(stdin);
        switch (FSM_State){
            case FSM_Start:
                if(c == '"')
                    FSM_State = FSM_String;
                else if((c == '_') || ((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')))
                    FSM_State = FSM_ID;
                else if((c >= '0') && (c <= '9'))
                    FSM_State = FSM_Int;
                else if(c == '+'){
                    NewToken->token = TOKEN_Plus;
                    NewToken->line = line;
                    done = true;
                }
                else if(c == '-')
                    FSM_State = FSM_Minus;
                else if(c == '*'){
                    NewToken->token = TOKEN_Mul;
                    NewToken->line = line;
                    done = true;
                }
                else if(c == '/')
                    FSM_State = FSM_Div;
                else if(c == '#'){
                    NewToken->token = TOKEN_StrLen;
                    NewToken->line = line;
                    done = true;
                }
                else if(c == ':'){
                    NewToken->token = TOKEN_Define;
                    NewToken->line = line;
                    done = true;
                }
                else if(c == '('){
                    NewToken->token = TOKEN_LeftPar;
                    NewToken->line = line;
                    done = true;
                }
                else if(c == ')'){
                    NewToken->token = TOKEN_RightPar;
                    NewToken->line = line;
                    done = true;
                }
                else if(c == ','){
                    NewToken->token = TOKEN_Separator;
                    NewToken->line = line;
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
                    line++;
                else if(c == EOF){
                    NewToken->token = TOKEN_EOF;
                    NewToken->line = line;
                    done = true;
                }
                break;
            case FSM_String:
                if(c == '"'){
                    if((NewToken->content.str = malloc(sizeof((strlen(newInput.str)) + 1))) == NULL)
                        error = true;
                    else{
                        NewToken->content.str = strcpy(NewToken->content.str, newInput.str);
                        NewToken->content.str = strcat(NewToken->content.str,"\0");
                        NewToken->token = TOKEN_String;
                    }
                    done = true;
                }
                else if(c == '\\'){
                    if(InsertChar(&newInput, &c) != 0)
                        error = true;
                    FSM_State = FSM_StrEsc;
                }
                else if(c == ' '){
                    if(InsertChar(&newInput, "\\032") != 0)
                        error = true;
                }
                else if(c == '#'){
                    if(InsertChar(&newInput, "\\035") != 0)
                        error = true;
                }
                else if((c >= 31) && (c <= 255))
                    if(InsertChar(&newInput, &c) != 0)
                        error = true;
                break;
            case FSM_StrEsc:
                if(c == '"'){
                    if(InsertChar(&newInput, "034") != 0)
                        error = true;
                    FSM_State = FSM_String;
                }
                else if(c == '\\'){
                    if(InsertChar(&newInput, "092") != 0)
                        error = true;
                    FSM_State = FSM_String;
                }
                else if(c == 'n'){
                    if(InsertChar(&newInput, "010") != 0)
                        error = true;
                    FSM_State = FSM_String;
                }
                else if(c == 't'){
                    if(InsertChar(&newInput, "009") != 0)
                        error = true;
                    FSM_State = FSM_String;
                }
                else if((c == '0') || (c == '1')){
                    if(InsertChar(&newInput, c) != 0)
                        error = true;
                    FSM_State = FSM_StrEscA;
                }
                else if(c == '2'){
                    if(InsertChar(&newInput, c) != 0)
                        error = true;
                    FSM_State = FSM_StrEscB;
                }
                else
                    error = true;
                break;
            case FSM_StrEscA:
                if((c >= 0) && (c <= 9)){
                    if(InsertChar(&newInput, c) != 0)
                        error = true;
                    FSM_State = FSM_StrEscC;
                }
                else
                    error = true;
                break;
            case FSM_StrEscB:
                if((c >= 0) && (c <= 4)){
                    if(InsertChar(&newInput, c) != 0)
                        error = true;
                    FSM_State = FSM_StrEscC;
                }
                if(c == 5){
                    if(InsertChar(&newInput, c) != 0)
                        error = true;
                    FSM_State = FSM_StrEscD;
                }
                else
                    error = true;
                break;
            case FSM_StrEscC:
                if((c >= 0) && (c <= 9)){
                    if(InsertChar(&newInput, c) != 0)
                        error = true;
                    FSM_State = FSM_String;
                }
                else
                    error = true;
                break;
            case FSM_StrEscD:
                if((c >= 0) && (c <= 5)){
                    if(InsertChar(&newInput, c) != 0)
                        error = true;
                    FSM_State = FSM_String;
                }
                else
                    error = true;
                break;
            case FSM_ID:
                if(((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) || ((c >= '0') && (c <= '9')) || (c == '_')){
                    if(InsertChar(&newInput, c) != 0)
                        error = true;
                }
                else{
                    NewToken->token = isKeyWord(&newInput);
                    if(NewToken->token == TOKEN_ID){
                        if((NewToken->content.str = malloc(sizeof((strlen(newInput.str)) + 1))) == NULL)
                            error = true;
                        else{
                            NewToken->content.str = strcpy(NewToken->content.str, newInput.str);
                            NewToken->content.str = strcat(NewToken->content.str,"\0");
                        }
                    }
                    done = true;
                }
                break;
            default:
                break;
        }
    }
    free(newInput.str);
    if(error){
        free(NewToken);
        return NULL;
    }
    return NewToken;
}

// --End of file--