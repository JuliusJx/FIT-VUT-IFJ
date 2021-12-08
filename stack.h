/*
 * File: stack.h
 * Header file for stack.c
 * Subject: IFJ
 * Project: Implementace prekladace imperativniho jazyka IFJ21
 * Authors: Milan Hrabovský (xhrabo15), Ondrej Kovac (xkovac57), Jakub Julius Smykal (xsmyka01), Martin Talajka (xtalaj00)
 * Year: 2021
 */

#ifndef stack_h_
#define stack_h_

#include <stdio.h>
#include <stdbool.h>

//Structure for items in stack
typedef struct StackItem{
    int value; //token type / scope index
    struct StackItem *next;
}StackItem;

//Structure for stack
typedef struct stack{
    StackItem *top;
}stack;

void stackInit( stack*);

bool stackPush( stack*, int);

bool stackTop( stack*, int*);

bool stackPop( stack*, int*);

bool stackIsEmpty( stack*);

#endif

// --End of file--