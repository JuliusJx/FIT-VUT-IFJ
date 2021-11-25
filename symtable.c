/*
 * File: symtable.h
 * Header file for symtable.c
 * Subject: IFJ
 * Authors: Jakub Julius Smykal, Ondrej Kovac, Martin Talajka, Milan Hrabovsky
 * Year: 2021
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "symtable.h"

//Function initializes symbol table
void symInit( symTable *table){
    for(int i = 0; i < SYMTABLE_SIZE; i++){
        (*table)[i] = NULL;
    }
}

//Function calculates index from char array
//Function returns said index
int symGetHash( char *id){
    int sum = 0, i = 0;
    while(id[i] != '\0'){
        sum += id[i];
        i++;
    }
    return sum % SYMTABLE_SIZE;
}
//Function searches for item based on given id and scope value
//Function returns pointer to item if found, otherwise returns null
tableItem *symGetItem( symTable *table, char *id, unsigned short scope){
    int index = symGetHash(id);
    tableItem *item;
    while (true){
        item = (*table)[index];
        while(item != NULL){
            if((!strcmp(item->name, id)) && (item->scope == scope))
                return item;
            item = item->next;
        }
        if(scope == 0)
            break;
        scope--;
    }
    return NULL;
}
//Function removes item based on given id and scope value
//Function does nothing if cannot find item
void symDelete( symTable *table, char *id, unsigned short scope){
    int index = symGetHash(id);
    tableItem *item;
    item = (*table)[index];
    if(item != NULL){
        //If first item on index is the item that is being deleted
        if((!strcmp(item->name, id)) && (item->scope == scope)){
            (*table)[index] = item->next;
            free(item->name);
            free(item);
        }
        //Searches in linked list of items on given index
        else{
            while(item->next != NULL){
                if((!strcmp(item->next->name, id)) && (item->next->scope == scope)){
                    tableItem *tmp = item->next;
                    item->next = item->next->next;
                    free(tmp->name);
                    free(tmp);
                    break;
                }
                item = item->next;
            }
        }
    }
}
//Function removes all items from symbol table
void symDeleteAll( symTable *table){
    tableItem *tmp;
    for(int i = 0; i < SYMTABLE_SIZE; i++){
        while ((*table)[i] != NULL){
            tmp = (*table)[i];
            (*table)[i] = tmp->next;
            if(tmp->paramAmount > 0)
                free(tmp->paramTypes);
            if(tmp->returnAmount > 0)
                free(tmp->returnTypes);
            free(tmp->name);
            free(tmp);
        }
    }
}


void symDeleteScope( symTable *table, int scope){
    tableItem *tmp0, *tmp1;
    for(int i = 0; i < SYMTABLE_SIZE; i++){
        tmp0 = (*table)[i];
        if(tmp0 == NULL)
            continue;
        if(tmp0->scope == scope){
            (*table)[i] = tmp0->next;
            if(tmp0->paramAmount > 0)
                free(tmp0->paramTypes);
            if(tmp0->returnAmount > 0)
                free(tmp0->returnTypes);
            free(tmp0->name);
            free(tmp0);
            i--;
            continue;
        }
        while (tmp0->next != NULL){
            if(tmp0->next->scope == scope){
                tmp1 = tmp0->next;
                tmp0->next = tmp0->next->next;
                if(tmp1->paramAmount > 0)
                    free(tmp1->paramTypes);
                if(tmp1->returnAmount > 0)
                    free(tmp1->returnTypes);
                free(tmp1->name);
                free(tmp1);
            }
            else
                tmp0 = tmp0->next;
        }
    }
}
//Function creates new item, sets its values and adds new item to the table
//Function returns true if insert was successful, otherwise returns false
bool symInsert( symTable *table, char *id, unsigned short type, bool isInit, unsigned short scope){
    int index = symGetHash(id);
    tableItem *newItem;
    if((newItem = malloc(sizeof(tableItem))) == NULL)
        return false;
    if((newItem->name = malloc(strlen(id) + 1)) == NULL){
        free(newItem);
        return false;
    }
    //Sets all values in new item
    newItem->name = strcpy(newItem->name, id);
    newItem->type = type;
    newItem->isInit = isInit;
    newItem->isUsed = false;
    newItem->paramAmount = 0;
    newItem->returnAmount = 0;
    newItem->paramTypes = NULL;
    newItem->returnTypes = NULL;
    newItem->scope = scope;
    newItem->next = (*table)[index];
    (*table)[index] = newItem;
    return true;
}
//Function adds new parameter to function's log in symbol table
//Function returns true if successful, otherwise returns false
bool symAddParam( tableItem *item, char paramType){
    if(item == NULL)
        return false;
    //If list of parameters is empty
    if(item->paramTypes == NULL){
        if((item->paramTypes = malloc(2 * sizeof(char))) == NULL)
            return false;
        sprintf(&(item->paramTypes[0]), "%d", paramType);
        item->paramAmount++;
    }
    else{
                               //needs one space for new character and one for terminal character
        if((item->paramTypes = realloc(item->paramTypes, strlen(item->paramTypes) + 2)) == NULL)
            return false;
        sprintf(&(item->paramTypes[item->paramAmount++]), "%d", paramType);
    }
    return true;
}
//Function adds new return to function's log in symbol table
//Function returns true if successful, otherwise returns false
bool symAddReturn( tableItem *item, char returnType){
    if(item == NULL)
        return false;
    //If list of returns is empty
    if(item->returnTypes == NULL){
        if((item->returnTypes = malloc(2 * sizeof(char))) == NULL)
            return false;
        sprintf(&(item->returnTypes[0]), "%d", returnType);
        item->returnAmount++;
    }
    else{
                            //needs one space for new character and one for terminal character
        if((item->returnTypes = realloc(item->returnTypes, strlen(item->returnTypes) + 2)) == NULL)
            return false;
        sprintf(&(item->returnTypes[item->returnAmount++]), "%d", returnType);
    }
    return true;
}

void symToggleInit( symTable *table, char *id, unsigned short scope){
    tableItem *tmp = symGetItem(table, id, scope);
    tmp->isInit = true;
}

void symToggleUsed( symTable *table, char *id, unsigned short scope){
    tableItem *tmp = symGetItem(table, id, scope);
    tmp->isUsed = true;
}

// --End of file--