/*
 * File: str_stack.c
 * Stack for bottom-up parser
 * Subject: IFJ
 * Project: Implementace prekladace imperativniho jazyka IFJ21
 * Authors: Milan Hrabovský (xhrabo15), Ondrej Kovac (xkovac57), Jakub Julius Smykal (xsmyka01), Martin Talajka (xtalaj00)
 * Year: 2021
 */

#include "str_stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

//Function resets top pointer to null
void s_stackInit( s_stack *stack){
    stack->top = NULL;
}
//Function pushes item into the stack
//Function returns true if successful, otherwise returns false
bool s_stackPush( s_stack *stack, char *str){
    s_StackItem *item;
    if((item = malloc(sizeof(s_StackItem))) == NULL)
        return false;
    if((item->str = malloc(strlen(str)+1)) == NULL)
        return false;
    item->next = stack->top;
    item->str = strcpy(item->str, str);
    stack->top = item;
    return true;
}
//Function sets target value to stack->top value if stack is not empty
//Function returns true if successful, otherwise returns false
bool s_stackTop( s_stack *stack, char **target){
    if(!s_stackIsEmpty(stack)){
        *target = stack->top->str;
        return true;
    }
    return false;
}
//Function sets target value to stack->top value if stack is not empty
//Function removes top item from stack
//Function returns true if successful, otherwise returns false
bool s_stackPop( s_stack *stack, char **target){
    if(!s_stackIsEmpty(stack)){
        *target = stack->top->str;
        s_StackItem *tmp = stack->top;
        stack->top = stack->top->next;
        free(tmp);
        return true;
    }
    return false;
}
//Function returns true if stack is empty, otherwise returns false
bool s_stackIsEmpty( s_stack *stack){
    return stack->top == NULL;
}
// --End of file--