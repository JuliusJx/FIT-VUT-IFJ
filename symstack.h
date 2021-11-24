/*
 * File: symstack.h
 * Header file for symstack.c
 * Subject: IFJ
 * Authors: Jakub Julius Smykal, Ondrej Kovac
 * Year: 2021
 */

#ifndef symstack_h_
#define symstack_h_

#include <stdio.h>
#include <stdbool.h>
#include "symtable.h"

//Structure for items in stack
typedef struct symstackItem{
    tableItem *item;
    struct StackItem *next;
}symstackItem;

//Structure for stack
typedef struct symstack{
    int count;
    symstackItem *top;
}symstack;

void symstackInit( symstack*);

bool symstackPush( symstack*, tableItem*);

bool symstackTop( symstack*, tableItem**);

bool symstackPop( symstack*, tableItem**);

int symstackCount( symstack*);

bool symstackIsEmpty( symstack*);

#endif

// --End of file--