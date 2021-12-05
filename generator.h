/*
 * File: generator.h
 * Header file for generator.c
 * Subject: IFJ
 * Authors: Jakub Julius Smykal, Ondrej Kovac
 * Year: 2021
 */

#ifndef generator_h_
#define generator_h_

#include <stdio.h>
#include <stdbool.h>
#include "scanner.h"
#include "err_handler.h"
#include "symtable.h"
#include "parser.h"

#define GEN_CODE(buffer,string) if(!insertString((buffer),(string))){errCode = 99; return false;}

extern contentInput startBuffer;
extern contentInput defBuffer;
extern contentInput blockBuffer;
extern contentInput callBuffer;
extern contentInput postCallBuffer;

bool mallocBuffer( contentInput*);

bool mallocBuffers();

void freeBuffer( contentInput*);

void freeBuffers();

bool insertString( contentInput*, char*);

bool genToInteger();

bool genSubstr();

bool genOrd();

bool genChr();

bool genCallArgLit( contentInput*, int, token*);

bool genWriteLit( contentInput*, token*);

bool genCallArgID( contentInput*, int, tableItem*);

bool genWriteID( contentInput*, tableItem*);

bool genVar( contentInput*, tableItem*);

void generateCode();

#endif

// --End of file-