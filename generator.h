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

#define GEN_DEF(_code) if(!insertString(&defBuffer,(_code))){errCode = 99; return false;}
#define GEN_CALL(_code) if(!insertString(&callBuffer,(_code))){errCode = 99; return false;}

extern contentInput defBuffer;
extern contentInput finalBuffer;
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

bool genCallArg( contentInput*, int, token*);

#endif

// --End of file-