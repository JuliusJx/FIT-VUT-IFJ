/*
 * File: stack.h
 * Header file for stack.c
 * Subject: IFJ
 * Authors: Jakub Julius Smykal, Ondrej Kovac, Martin Talajka, Milan Hrabovsky
 * Year: 2021
 */

#ifndef stack_h_
#define stack_h_

//Structure for items in stack
typedef struct  item{
    int value; //token type / scope index
    item *next;
}StackItem;

//Structure for stack
typedef struct stack{
    item *top;
}stack;

void stackInit( stack*);

bool stackPush( stack*, int);

bool stackTop( stack*, int*);

bool stackPop( stack*, int*);

bool stackIsEmpty( stack*);

#endif

// --End of file--