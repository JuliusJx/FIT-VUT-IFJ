/*
 * File: symstack.h
 * Header file for symstack.c
 * Subject: IFJ
 * Project: Implementace prekladace imperativniho jazyka IFJ21
 * Authors: Milan Hrabovský (xhrabo15), Ondrej Kovac (xkovac57), Jakub Julius Smykal (xsmyka01), Martin Talajka (xtalaj00)
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
    struct symstackItem *next;
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