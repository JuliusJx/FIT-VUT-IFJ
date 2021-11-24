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
    symstackItem *top;
}symstack;

void symstackInit( symstackItem*);

bool symstackPush( symstackItem*, tableItem*);

bool symstackTop( symstackItem*, tableItem**);

bool symstackPop( symstackItem*, tableItem**);

bool symstackIsEmpty( symstackItem*);

#endif

// --End of file--