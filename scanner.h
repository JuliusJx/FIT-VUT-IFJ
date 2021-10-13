/*
 * File: scanner.h
 * Header file for scanner.c
 * Subject: IFJ
 * Authors: Jakub Julius Smykal
 * Year: 2021
 */

#ifndef scanner_h_
#define scanner_h_

#define FSM_Start 0
#define FSM_StrArr 1        // "
#define FSM_ID 2            // _
#define FSM_IDorKey 3       // a/A..z/Z
#define FSM_Number 4        // 0..9
#define FSM_Float 5         // ./e
#define FSM_Operator 6      // Arithmetic/Relation operators
#define FSM_Comment 7       // --
#define FSM_MultLComment 8  // --[[
#define FSM_CheckKeyWords 9
#define FSM_End 10

typedef struct token{
    char *content;
    char *id;
    unsigned line;
}token;

token *GetToken();

void IDkeyWord( token*);

#endif

// --End of file--