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

#define true 1
#define false 0

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


token *GetToken(){
    token *NewToken;
    if((NewToken = malloc(sizeof(token))) == NULL)
        return NULL;
    
    contentInput newInput = {.index = 0, .length = 8};
    if((newInput.str = malloc(newInput.length)) == NULL)
        return NULL;

    char c;
    short done = 0;
    unsigned line = 1;
    unsigned short FSM_State = FSM_Start;

    while(!done){
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
                    //copy newInput to token->str
                    NewToken->token = TOKEN_String;
                    done = true;
                }
                else if(c == '\\'){
                    if(InsertChar(&newInput, &c) != 0)
                        return NULL;
                    FSM_State = FSM_StrEsc;
                }
                else if(c == ' '){
                    if(InsertChar(&newInput, "\\032") != 0)
                        return NULL;
                }
                else if(c == '#'){
                    if(InsertChar(&newInput, "\\035") != 0)
                        return NULL;
                }
                else if((c >= 31) && (c <= 255))
                    if(InsertChar(&newInput, &c) != 0)
                        return NULL;
                break;
            case FSM_StrEsc:
                if(c == '"'){
                    if(InsertChar(&newInput, "034") != 0)
                        return NULL;
                    FSM_State = FSM_String;
                }
                else if(c == '\\'){
                    if(InsertChar(&newInput, "092") != 0)
                        return NULL;
                    FSM_State = FSM_String;
                }
                else if(c == 'n'){
                    if(InsertChar(&newInput, "010") != 0)
                        return NULL;
                    FSM_State = FSM_String;
                }
                else if(c == 't'){
                    if(InsertChar(&newInput, "009") != 0)
                        return NULL;
                    FSM_State = FSM_String;
                }
                else if((c == '0') || (c == '1')){
                    if(InsertChar(&newInput, c) != 0)
                        return NULL;
                    FSM_State = FSM_StrEscA;
                }
                else if(c == '2'){
                    if(InsertChar(&newInput, c) != 0)
                        return NULL;
                    FSM_State = FSM_StrEscB;
                }
                else
                    return NULL;
            default:
                break;
        }
    }
    return NewToken;
}

// --End of file--