/*
 * File: scanner.c
 * c file for lexical analysis
 * Subject: IFJ
 * Authors: Jakub Julius Smykal
 * Year: 2021
 */

#include "stdio.h"
#include "scanner.h"

#define true 1
#define false 0

token *GetToken(){
    token *NewToken;
    if((NewToken = malloc(sizeof(token))) == NULL)
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
        }
    }
    return NewToken;
}

// --End of file--