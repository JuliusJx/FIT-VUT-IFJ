/*
 * File: scanner.c
 * Subject: IFJ
 * Authors: Jakub Julius Smykal
 * Year: 2021
 */

#include "stdio.h"
#include "scanner.h"

token *GetToken(){
    token NewToken;
    unsigned FSM_State = FSM_Start;
    char c;

    while(FSM_State != FSM_End){
        c = getc(stdin);
        switch (FSM_State){
            case FSM_Start:
                if(c == '"')
                    FSM_State = FSM_String;
                else if(c == '_')
                    FSM_State = FSM_ID;
                else if(((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')))
                    FSM_State = FSM_IDorKey;
                else if((c >= '0') && (c <= '9'))
                    FSM_State = FSM_Int;
                else if((c == '(') || (c == ')') || (c == ','))
                    FSM_State = FSM_Separator;
                else if((c == '#') || (c == ":") || (c == '+') || (c == '*') 
                        || (c == '/') || (c == '<') || (c == '=') || (c == '>'))
                    FSM_State = FSM_Operator;
                else if(c == '-'){
                    //checks if next character is -, if yes it is comment
                    char c2 = getc(stdin);
                    if(c2 == '-'){
                        //Gets next 2 characters and checks if it is multi line comment
                        c2 = getc(stdin);
                        char c3 = getc(stdin);
                        if((c2 == '[')&&(c3 == '[')){
                            FSM_State = FSM_MultLComment;
                        }
                        else{
                            //Returns characters if it is not multiline comment
                            //otherwise could cause problems with short comments
                            ungetc(c3,stdin);
                            ungetc(c2,stdin);
                        }
                        FSM_State = FSM_Comment;
                    }
                    else
                        FSM_State = FSM_Operator;
                    ungetc(c2, stdin);
                }
            default:
        }
    }


}

// --End of file--