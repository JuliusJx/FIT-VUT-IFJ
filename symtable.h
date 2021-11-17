/*
 * File: symtable.h
 * Header file for symtable.c
 * Subject: IFJ
 * Authors: Jakub Julius Smykal, Ondrej Kovac, Martin Talajka, Milan Hrabovsky
 * Year: 2021
 */

#ifndef stack_h_
#define stack_h_

#include <stdio.h>
#include <stdbool.h>

#define SYMTABLE_SIZE 100

#define FUNC_ID 0
#define TYPE_INT 1
#define TYPE_NUM 2
#define TYPE_STR 3

//Structure for symbol table items
typedef struct tableItem{
    char *name;                         //name of function/variable
    unsigned short type;                //defined at lines 17-20
    bool isInit;                        //shared bool between variables (true if initialized) and functions (true if defined) 
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

void symDeleteAll( symTable*);

bool symInsert( symTable*, char*, unsigned short, bool, unsigned short);

bool symAddParam( tableItem*, char);

bool symAddReturn( tableItem*, char);

#endif

// --End of file--