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