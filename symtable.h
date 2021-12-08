/*
 * File: symtable.h
 * Header file for symtable.c
 * Subject: IFJ
 * Project: Implementace prekladace imperativniho jazyka IFJ21
 * Authors: Milan Hrabovský (xhrabo15), Ondrej Kovac (xkovac57), Jakub Julius Smykal (xsmyka01), Martin Talajka (xtalaj00)
 * Year: 2021
 */

#ifndef symtable_h_
#define symtable_h_

#include <stdio.h>
#include <stdbool.h>

#define SYMTABLE_SIZE 100

#define FUNC_ID 0
#define TYPE_INT 1
#define TYPE_NUM 2
#define TYPE_STR 3
#define TYPE_NIL 4

//Structure for symbol table items
typedef struct tableItem{
    char *name;                         //name of function/variable
    unsigned short type;                //defined at lines 17-20
    bool isInit;
    bool isUsed;                        //shared bool between variables (true if initialized) and functions (true if defined) 
    unsigned short paramAmount;         //paramTypes/returnTypes structure:
    unsigned short returnAmount;        //variable types are represented by numbers
    char *paramTypes;                   //order of types is represented by string of said numbers
    char *returnTypes;                  //e.g. (string, int, int, float) is stored as "3112"
    unsigned short scope;               //represents block of code, where item is declared, implemented using stack
    struct tableItem *next;
}tableItem;

//typedef for symbol table array
typedef tableItem *symTable[SYMTABLE_SIZE];


void symInit( symTable*);

int symGetHash( char*);

tableItem *symGetItem( symTable*, char*, unsigned short);

void symDelete( symTable*, char*, unsigned short);
//add final check (is used, is init)
void symDeleteAll( symTable*);

void symDeleteScope( symTable*, int);

bool symInsert( symTable*, char*, unsigned short, bool, unsigned short);

bool symAddParam( tableItem*, char);

bool symAddReturn( tableItem*, char);

void symToggleInit( symTable*, char*, unsigned short);

void symToggleUsed( symTable*, char*, unsigned short);

#endif

// --End of file--