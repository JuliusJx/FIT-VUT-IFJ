/*
 * File: stack.c
 * Stack for bottom-up parser and scope
 * Subject: IFJ
 * Project: Implementace prekladace imperativniho jazyka IFJ21
 * Authors: Milan Hrabovský (xhrabo15), Ondrej Kovac (xkovac57), Jakub Julius Smykal (xsmyka01), Martin Talajka (xtalaj00)
 * Year: 2021
 */

#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//Function resets top pointer to null
void stackInit( stack *stack){
    stack->top = NULL;
}
//Function pushes item into the stack
//Function returns true if successful, otherwise returns false
bool stackPush( stack *stack, int value){
    StackItem *item;
    if((item = malloc(sizeof(StackItem))) == NULL)
        return false;
    item->next = stack->top;
    item->value = value;
    stack->top = item;
    return true;
}
//Function sets target value to stack->top value if stack is not empty
//Function returns true if successful, otherwise returns false
bool stackTop( stack *stack, int *target){
    if(!stackIsEmpty(stack)){
        *target = stack->top->value;
        return true;
    }
    return false;
}
//Function sets target value to stack->top value if stack is not empty
//Function removes top item from stack
//Function returns true if successful, otherwise returns false
bool stackPop( stack *stack, int *target){
    if(!stackIsEmpty(stack)){
        *target = stack->top->value;
        StackItem *tmp = stack->top;
        stack->top = stack->top->next;
        free(tmp);
        return true;
    }
    return false;
}
//Function returns true if stack is empty, otherwise returns false
bool stackIsEmpty( stack *stack){
    return stack->top == NULL;
}

// --End of file--