#include "stdio.h"
#include "string.h"
#include "../symtable.h"
#include "stdlib.h"

// gcc -Wall -std=c11 -pedantic -lm -g -o symtable_test symtable_test.c ../symtable.c
int main(){
    symTable *table = malloc(sizeof(symTable));
    tableItem *item = NULL;
    int result = 0;

    // Test "symInit( symTable*)"
    symInit(table);

    // Test "int symGetHash( char*)"
    printf("| TEST | symGetHash(\"first\") \n");
    result = symGetHash("first");
    printf(" - Return: %d\n\n", result);

    // Test "tableItem *symGetItem( symTable*, char*, unsigned short)"
    printf("| TEST | symGetItem(table, \"first\", 0) \n");
    item = symGetItem(table, "first", 0);
    if(item != NULL)
        printf(" - Return: not NULL\n\n");
    else
        printf(" - Return: NULL\n\n");

    // Test "symInsert( symTable*, char*, unsigned short, bool, unsigned short)"
    printf("| TEST | symInsert(table, \"first\", FUNC_ID, true, 12) \n");
    result = symInsert(table, "first", FUNC_ID, true, 12);
    printf(" - Return: %d\n", result);

    int index = symGetHash("first");
    item = (*table)[index];

    printf(" - Item: %s\n", item->name);
    printf("  - Type: %d\n", item->type);
    printf("  - isInit: %d\n", item->isInit);
    printf("  - paramAmount: %d\n", item->paramAmount);
    printf("  - returnAmount: %d\n", item->returnAmount);
    if(item->paramTypes == NULL)
        printf("  - paramTypes: NULL\n");
    else
        printf("  - paramTypes: ??");
    if(item->returnTypes == NULL)
        printf("  - paramTypes: NULL\n");
    else
        printf("  - paramTypes: ??");
    printf("  - scope: %d\n", item->scope);
    printf("\n");

    // Test "tableItem *symGetItem( symTable*, char*, unsigned short)"
    printf("| TEST | symGetItem(table, \"first\", 0) \n");
    item = symGetItem(table, "first", 0);
    if(item != NULL)
        printf(" - Return: not NULL\n\n");
    else
        printf(" - Return: NULL\n\n");

    // Test "tableItem *symGetItem( symTable*, char*, unsigned short)"
    printf("| TEST | symGetItem(table, \"first\", 12) \n");
    item = symGetItem(table, "first", 12);
    if(item != NULL){
        printf(" - Return: not NULL\n");
        printf(" - Item: %s\n", item->name);
        printf("  - Type: %d\n", item->type);
        printf("  - isInit: %d\n", item->isInit);
        printf("  - paramAmount: %d\n", item->paramAmount);
        printf("  - returnAmount: %d\n", item->returnAmount);
        if(item->paramTypes == NULL)
            printf("  - paramTypes: NULL\n");
        else
            printf("  - paramTypes: ??");
        if(item->returnTypes == NULL)
            printf("  - paramTypes: NULL\n");
        else
            printf("  - paramTypes: ??");
        printf("  - scope: %d\n", item->scope);
        printf("\n");
    }
    else
        printf(" - Return: NULL\n\n");

    // Test "tableItem *symGetItem( symTable*, char*, unsigned short)"
    printf("| TEST | symGetItem(table, \"test\", 12) \n");
    item = symGetItem(table, "test", 12);
    if(item != NULL)
        printf(" - Return: not NULL\n\n");
    else
        printf(" - Return: NULL\n\n");

    // Test "symInsert( symTable*, char*, unsigned short, bool, unsigned short)"
    printf("| TEST | symInsert(table, \"second\", TYPE_NUM, true, 2) \n");
    result = symInsert(table, "second", TYPE_NUM, true, 2);
    printf(" - Return: %d\n\n", result);

    // Test "symInsert( symTable*, char*, unsigned short, bool, unsigned short)"
    printf("| TEST | symInsert(table, \"third\", TYPE_STR, true, 8) \n");
    result = symInsert(table, "third", TYPE_STR, true, 8);
    printf(" - Return: %d\n\n", result);

    // Test "tableItem *symGetItem( symTable*, char*, unsigned short)"
    printf("| TEST | symGetItem(table, \"third\", 8) \n");
    item = symGetItem(table, "third", 8);
    if(item != NULL){
        printf(" - Return: not NULL\n");
        printf(" - Item: %s\n", item->name);
        printf("  - Type: %d\n", item->type);
        printf("  - isInit: %d\n", item->isInit);
        printf("  - paramAmount: %d\n", item->paramAmount);
        printf("  - returnAmount: %d\n", item->returnAmount);
        if(item->paramTypes == NULL)
            printf("  - paramTypes: NULL\n");
        else
            printf("  - paramTypes: ??");
        if(item->returnTypes == NULL)
            printf("  - paramTypes: NULL\n");
        else
            printf("  - paramTypes: ??");
        printf("  - scope: %d\n", item->scope);
        printf("\n");
    }
    else
        printf(" - Return: NULL\n\n");

    // Test "tableItem *symGetItem( symTable*, char*, unsigned short)"
    printf("| TEST | symGetItem(table, \"second\", 2) \n");
    item = symGetItem(table, "second", 2);
    if(item != NULL){
        printf(" - Return: not NULL\n");
        printf(" - Item: %s\n", item->name);
        printf("  - Type: %d\n", item->type);
        printf("  - isInit: %d\n", item->isInit);
        printf("  - paramAmount: %d\n", item->paramAmount);
        printf("  - returnAmount: %d\n", item->returnAmount);
        if(item->paramTypes == NULL)
            printf("  - paramTypes: NULL\n");
        else
            printf("  - paramTypes: ??");
        if(item->returnTypes == NULL)
            printf("  - paramTypes: NULL\n");
        else
            printf("  - paramTypes: ??");
        printf("  - scope: %d\n", item->scope);
        printf("\n");
    }
    else
        printf(" - Return: NULL\n\n");

    // Test "symDelete( symTable*, char*, unsigned short)"
    printf("| TEST | symDelete(table, \"second\", 2) \n");
    symDelete(table, "second", 2);

    // Test "tableItem *symGetItem( symTable*, char*, unsigned short)"
    printf("| TEST | symGetItem(table, \"second\", 2) \n");
    item = symGetItem(table, "second", 2);
    if(item != NULL)
        printf(" - Return: not NULL\n");
    else
        printf(" - Return: NULL\n\n");

    // Test "symAddParam( tableItem*, char)"
    printf("| TEST | symAddParam(table, \"first\", TYPE_STR) \n");
    item = symGetItem(table, "first", 12);
    result = symAddParam(item, TYPE_STR);
    if(result == false)
        printf(" - Return: 0\n\n");
    else {
        printf(" - Return: %d\n", result);
        printf(" - Item: %s\n", item->name);
        printf("  - Type: %d\n", item->type);
        printf("  - isInit: %d\n", item->isInit);
        printf("  - paramAmount: %d\n", item->paramAmount);
        printf("  - returnAmount: %d\n", item->returnAmount);
        if(item->paramTypes == NULL)
            printf("  - paramTypes: NULL\n");
        else
            printf("  - paramTypes: %s\n", item->paramTypes);
        if(item->returnTypes == NULL)
            printf("  - returnTypes: NULL\n");
        else
            printf("  - returnTypes: %s\n", item->returnTypes);
        printf("  - scope: %d\n", item->scope);
        printf("\n");
    }

    // Test "symAddReturn( tableItem*, char)"
    printf("| TEST | symAddReturn(table, \"first\", TYPE_INT) \n");
    item = symGetItem(table, "first", 12);
    result = symAddReturn(item, TYPE_INT);
    if(result == false)
        printf(" - Return: 0\n\n");
    else {
        printf(" - Return: %d\n", result);
        printf(" - Item: %s\n", item->name);
        printf("  - Type: %d\n", item->type);
        printf("  - isInit: %d\n", item->isInit);
        printf("  - paramAmount: %d\n", item->paramAmount);
        printf("  - returnAmount: %d\n", item->returnAmount);
        if(item->paramTypes == NULL)
            printf("  - paramTypes: NULL\n");
        else
            printf("  - paramTypes: %s\n", item->paramTypes);
        if(item->returnTypes == NULL)
            printf("  - returnTypes: NULL\n");
        else
            printf("  - returnTypes: %s\n", item->returnTypes);
        printf("  - scope: %d\n", item->scope);
        printf("\n");
    }

    // Test "symAddParam( tableItem*, char)"
    printf("| TEST | symAddParam(table, \"first\", TYPE_INT) \n");
    item = symGetItem(table, "first", 12);
    result = symAddParam(item, TYPE_INT);
    if(result == false)
        printf(" - Return: 0\n\n");
    else {
        printf(" - Return: %d\n", result);
        printf(" - Item: %s\n", item->name);
        printf("  - Type: %d\n", item->type);
        printf("  - isInit: %d\n", item->isInit);
        printf("  - paramAmount: %d\n", item->paramAmount);
        printf("  - returnAmount: %d\n", item->returnAmount);
        if(item->paramTypes == NULL)
            printf("  - paramTypes: NULL\n");
        else
            printf("  - paramTypes: %s\n", item->paramTypes);
        if(item->returnTypes == NULL)
            printf("  - returnTypes: NULL\n");
        else
            printf("  - returnTypes: %s\n", item->returnTypes);
        printf("  - scope: %d\n", item->scope);
        printf("\n");
    }

    // Test "symAddParam( tableItem*, char)"
    printf("| TEST | symAddParam(table, \"first\", TYPE_INT) \n");
    item = symGetItem(table, "first", 12);
    result = symAddParam(item, TYPE_INT);
    if(result == false)
        printf(" - Return: 0\n\n");
    else {
        printf(" - Return: %d\n", result);
        printf(" - Item: %s\n", item->name);
        printf("  - Type: %d\n", item->type);
        printf("  - isInit: %d\n", item->isInit);
        printf("  - paramAmount: %d\n", item->paramAmount);
        printf("  - returnAmount: %d\n", item->returnAmount);
        if(item->paramTypes == NULL)
            printf("  - paramTypes: NULL\n");
        else
            printf("  - paramTypes: %s\n", item->paramTypes);
        if(item->returnTypes == NULL)
            printf("  - returnTypes: NULL\n");
        else
            printf("  - returnTypes: %s\n", item->returnTypes);
        printf("  - scope: %d\n", item->scope);
        printf("\n");
    }

    // Test "symAddReturn( tableItem*, char)"
    printf("| TEST | symAddReturn(table, \"first\", TYPE_INT) \n");
    item = symGetItem(table, "first", 12);
    result = symAddReturn(item, TYPE_INT);
    if(result == false)
        printf(" - Return: 0\n\n");
    else {
        printf(" - Return: %d\n", result);
        printf(" - Item: %s\n", item->name);
        printf("  - Type: %d\n", item->type);
        printf("  - isInit: %d\n", item->isInit);
        printf("  - paramAmount: %d\n", item->paramAmount);
        printf("  - returnAmount: %d\n", item->returnAmount);
        if(item->paramTypes == NULL)
            printf("  - paramTypes: NULL\n");
        else
            printf("  - paramTypes: %s\n", item->paramTypes);
        if(item->returnTypes == NULL)
            printf("  - returnTypes: NULL\n");
        else
            printf("  - returnTypes: %s\n", item->returnTypes);
        printf("  - scope: %d\n", item->scope);
        printf("\n");
    }

    //Test "symDeleteAll( symTable*)"
    printf("| TEST | symDeleteAll(table) \n\n");
    symDeleteAll(table);

    //New tests --

    //Init and insert 6 items
    printf("| TEST | Init table and insert 6 items \n");
    symInit(table);
    symInsert(table, "first", FUNC_ID, true, 1);
    symInsert(table, "second", TYPE_NUM, true, 2);
    symInsert(table, "third", TYPE_INT, true, 3);
    symInsert(table, "tree", TYPE_STR, true, 4);
    symInsert(table, "car", TYPE_NUM, true, 5);
    symInsert(table, "house", FUNC_ID, true, 6);

    //Print all items
    for(int i = 0; i < SYMTABLE_SIZE; i++){
        tableItem *iitem = ((*table)[i]);
        if(iitem != NULL)
            printf("%d[%s]\n",i ,iitem->name);
    }
    printf("\n");

    printf("| TEST | Remove first, house, car, third\n");
    //Remove "first"
    symDelete(table, "first", 1);
    //Remove "house"
    symDelete(table, "house", 6);
    //Remove "car"
    symDelete(table, "car", 5);
    //Remove "third"
    symDelete(table, "third", 3);

    //Print all items
    for(int i = 0; i < SYMTABLE_SIZE; i++){
        tableItem *iitem = ((*table)[i]);
        if(iitem != NULL)
            printf("%d[%s]\n",i ,iitem->name);
    }
    printf("\n");

    //Add params and returns
    printf("| TEST | Add params (5x) and returns (5x)\n");
    item = symGetItem(table, "tree", 4);
    symAddParam(item, TYPE_INT);
    symAddParam(item, TYPE_INT);
    symAddParam(item, TYPE_NUM);
    symAddParam(item, TYPE_STR);
    symAddParam(item, FUNC_ID);
    symAddReturn(item, TYPE_INT);
    symAddReturn(item, TYPE_STR);
    symAddReturn(item, TYPE_NUM);
    symAddReturn(item, FUNC_ID);
    symAddReturn(item, TYPE_INT);
    printf(" - Item: %s\n", item->name);
    printf("  - paramAmount: %d\n", item->paramAmount);
    printf("   - paramTypes: %s\n", item->paramTypes);
    printf("  - returnAmount: %d\n", item->returnAmount);
    printf("   - returnTypes: %s\n", item->returnTypes);
    printf("\n");

    printf("| TEST | Remove all\n\n");
    symDeleteAll(table);

    free(table);
    printf("| DONE |\n");
    return 0;
}