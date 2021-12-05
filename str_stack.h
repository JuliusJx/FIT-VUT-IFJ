/*
 * File: str_stack.h
 * Header file for str_stack.c
 * Subject: IFJ
 * Authors: Jakub Julius Smykal, Ondrej Kovac, Martin Talajka, Milan Hrabovsky
 * Year: 2021
 */

#ifndef str_stack_h_
#define str_stack_h_

#include <stdio.h>
#include <stdbool.h>

//Structure for items in stack
typedef struct s_StackItem{
    char *str;
    struct s_StackItem *next;
}s_StackItem;

//Structure for stack
typedef struct s_stack{
    s_StackItem *top;
}s_stack;

void s_stackInit( s_stack*);

bool s_stackPush( s_stack*, char*);

bool s_stackTop( s_stack*, char**);

bool s_stackPop( s_stack*, char**);

bool s_stackIsEmpty( s_stack*);

#endif

// --End of file--