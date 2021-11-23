/*
 * File: scanner.h
 * Header file for scanner.c
 * Subject: IFJ
 * Authors: Jakub Julius Smykal, Ondrej Kovac, Martin Talajka
 * Year: 2021
 */

#ifndef scanner_h_
#define scanner_h_

#include <stdio.h>
#include <stdbool.h>

//Defines for FSM
#define FSM_Start 0

#define FSM_String 1 
#define FSM_StrEsc 2
#define FSM_StrEscA 3
#define FSM_StrEscB 4
#define FSM_StrEscC 5
#define FSM_StrEscD 6

#define FSM_ID 7

#define FSM_Int 8
#define FSM_NumDotA 9
#define FSM_NumDot 10
#define FSM_NumEA 11
#define FSM_NumEB 12
#define FSM_NumE 13

#define FSM_Minus 14
#define FSM_Div 15
#define FSM_Concat 16
#define FSM_Less 17
#define FSM_Greater 18
#define FSM_Assign 19
#define FSM_NotEqual 20

#define FSM_OneLineC 21
#define FSM_OneLineCA 22
#define FSM_OneLineOpenPar 23
#define FSM_MultiLineC 24
#define FSM_MultilineClosePar 25

//Defines for tokens
#define TOKEN_String 0
#define TOKEN_ID 1
#define TOKEN_Key_do 2
#define TOKEN_Key_else 3
#define TOKEN_Key_end 4
#define TOKEN_Key_function 5
#define TOKEN_Key_global 6
#define TOKEN_Key_if 7
#define TOKEN_Key_integer 8
#define TOKEN_Key_local 9
#define TOKEN_Key_nil 10
#define TOKEN_Key_number 11
#define TOKEN_Key_require 12
#define TOKEN_Key_return 13
#define TOKEN_Key_string 14
#define TOKEN_Key_then 15
#define TOKEN_Key_while 16
#define TOKEN_Int 17
#define TOKEN_Num 18
#define TOKEN_StrLen 19
#define TOKEN_Plus 20
#define TOKEN_Minus 21
#define TOKEN_Mul 22
#define TOKEN_Div 23
#define TOKEN_DivInt 24
#define TOKEN_Concat 25
#define TOKEN_Greater 26
#define TOKEN_GreatEQ 27
#define TOKEN_Less 28
#define TOKEN_LessEQ 29
#define TOKEN_Assign 30
#define TOKEN_EQ 31
#define TOKEN_NotEQ 32
#define TOKEN_Define 33
#define TOKEN_Separator 34
#define TOKEN_LeftPar 35
#define TOKEN_RightPar 36
#define TOKEN_Err 37
#define TOKEN_EOF 38

//Union structure for token's attributes
typedef union content{
    int i;
    float f;
    char *str;
} content;

//Token structure
typedef struct token{
    content content;
    unsigned short token;
    unsigned line;
} token;

//Structure for reading input
typedef struct contentInput{
    char *str;
    unsigned index;
    unsigned length;
} contentInput;

token *GetToken();

void IDkeyWord( token*);

void InsertChar( contentInput*, char, bool*);

int isKeyWord( contentInput*);

#endif

// --End of file--