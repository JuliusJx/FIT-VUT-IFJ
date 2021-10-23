/*
 * File: scanner.c
 * c file for lexical analysis
 * Subject: IFJ
 * Authors: Jakub Julius Smykal, Ondrej Kovac
 * Year: 2021
 */

#include "stdio.h"
#include "scanner.h"
#include "string.h"
#include "stdbool.h"
#include "stdlib.h"


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
    NewToken->line = 1;
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
                    NewToken->line++;
                else if(c == EOF){
                    NewToken->token = TOKEN_EOF;
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
                if((c >= '0') && (c <= '9')){
                    if(InsertChar(&newInput, c) != 0)
                        error = true;
                    FSM_State = FSM_StrEscC;
                }
                else
                    error = true;
                break;
            case FSM_StrEscB:
                if((c >= '0') && (c <= '4')){
                    if(InsertChar(&newInput, c) != 0)
                        error = true;
                    FSM_State = FSM_StrEscC;
                }
                if(c == '5'){
                    if(InsertChar(&newInput, c) != 0)
                        error = true;
                    FSM_State = FSM_StrEscD;
                }
                else
                    error = true;
                break;
            case FSM_StrEscC:
                if((c >= '0') && (c <= '9')){
                    if(InsertChar(&newInput, c) != 0)
                        error = true;
                    FSM_State = FSM_String;
                }
                else
                    error = true;
                break;
            case FSM_StrEscD:
                if((c >= '0') && (c <= '5')){
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
                    ungetc(c, stdin);
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
            case FSM_Int:
                if((c >= '0') && (c <= '9')){
                    if(InsertChar(&newInput, c) != 0)
                        error = true;
                }
                else if(c == '.'){
                    if(InsertChar(&newInput, c) != 0)
                        error = true;
                    FSM_State = FSM_NumDotA;
                }
                else if((c == 'e') || (c == 'E')){
                    if(InsertChar(&newInput, c) != 0)
                        error = true;
                    FSM_State = FSM_NumEA;
                }
                else if(((c >= 42) && (c <= 47)) || (c == ' ') || (c == '\n') || (c == EOF) || (c == '~') || ((c >= 60) && (c <= 62))){
                    ungetc(c, stdin);
                    NewToken->token = TOKEN_Int;
                    NewToken->content.i = strtoint(newInput.str);
                    done = true;
                }
                else
                    error = true;
                break;
            case FSM_NumDotA:
                if((c >= '0') && (c <= '9')){
                    if(InsertChar(&newInput, c) != 0)
                        error = true;
                    FSM_State = FSM_NumDot;
                }
                else
                    error = true;
                break;
            case FSM_NumDot:
                if((c == 'e') || (c == 'E')){
                    if(InsertChar(&newInput, c) != 0)
                        error = true;
                    FSM_State = FSM_NumEA;
                }
                else if((c >= '0') && (c <= '9')){
                    if(InsertChar(&newInput, c) != 0)
                        error = true;
                }
                else if(((c >= 42) && (c <= 47)) || (c == ' ') || (c == '\n') || (c == EOF) || (c == '~') || ((c >= 60) && (c <= 62))){
                    ungetc(c, stdin);
                    NewToken->token = TOKEN_Num;
                    NewToken->content.i = strtof(newInput.str, '\0');
                    done = true;
                }
                else
                    error = true;
                break;
            case FSM_NumEA:
                if ((c == '+') || (c == '-')){
                    if(InsertChar(&newInput, c) != 0)
                        error = true;
                    FSM_State = FSM_NumEB;
                }
                else if((c >= '0') && (c <= '9')){
                    if(InsertChar(&newInput, c) != 0)
                        error = true;
                    FSM_State = FSM_NumE;
                }
                else
                    error = true;
                break;
            case FSM_NumEB:
                if((c >= '0') && (c <= '9')){
                    if(InsertChar(&newInput, c) != 0)
                        error = true;
                    FSM_State = FSM_NumE;
                }
                else
                    error = true;
                break;
            case FSM_NumE:
                if((c >= '0') && (c <= '9')){
                    if(InsertChar(&newInput, c) != 0)
                        error = true;
                }
                else if(((c >= 42) && (c <= 47)) || (c == ' ') || (c == '\n') || (c == EOF) || (c == '~') || ((c >= 60) && (c <= 62))){
                    ungetc(c, stdin);
                    NewToken->token = TOKEN_Num;
                    NewToken->content.i = strtof(newInput.str, '\0');
                    done = true;
                }
                else
                    error = true;
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
                else if(c == EOF)
                    error = true;
                break;
            case FSM_MultilineClosePar:
                if(c == ']')
                    FSM_State = FSM_Start;
                else if(c == EOF)
                    error = true;
                else
                    FSM_State = FSM_MultiLineC;
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