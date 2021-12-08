/*
 * File: symstack.c
 * Stack for variable assignment
 * Subject: IFJ
 * Project: Implementace prekladace imperativniho jazyka IFJ21
 * Authors: Milan Hrabovský (xhrabo15), Ondrej Kovac (xkovac57), Jakub Julius Smykal (xsmyka01), Martin Talajka (xtalaj00)
 * Year: 2021
 */

#include "symstack.h"
#include "symtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//Function resets top pointer to null
void symstackInit( symstack *stack){
    stack->top = NULL;
    stack->count = 0;
}
//Function pushes item into the stack
//Function returns true if successful, otherwise returns false
bool symstackPush( symstack *stack, tableItem *item){
    symstackItem *newItem;
    if((newItem = malloc(sizeof(symstackItem))) == NULL)
        return false;
    newItem->next = stack->top;
    newItem->item = item;
    stack->top = newItem;
    stack->count++;
    return true;
}
//Function sets target pointer to stack->top pointer if stack is not empty
//Function returns true if successful, otherwise returns false
bool symstackTop( symstack *stack, tableItem **target){
    if(!symstackIsEmpty(stack)){
        *target = stack->top->item;
        return true;
    }
    return false;
}
//Function sets target pointer to stack->top pointer if stack is not empty
//Function removes top item from stack
//Function returns true if successful, otherwise returns false
bool symstackPop( symstack *stack, tableItem **target){
    if(!symstackIsEmpty(stack)){
        *target = stack->top->item;
        symstackItem *tmp = stack->top;
        stack->top = stack->top->next;
        free(tmp);
        stack->count--;
        return true;
    }
    return false;
}
//Function returns number of items stored in stack
int symstackCount( symstack *stack){
    return stack->count;
}
//Function returns true if stack is empty, otherwise returns false
bool symstackIsEmpty( symstack *stack){
    return stack->count == 0;
}

// --End of file--