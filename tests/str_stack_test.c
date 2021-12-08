#include "stdio.h"
#include "string.h"
#include "../str_stack.h"
#include "stdlib.h"
#include "stdbool.h"

// print stack
void print_stack(stack *stack) {
    printf("stack: \n");
    if(stack->top == NULL){
        printf("empty\n\n");
    }
    else {
        printf("%s ", stack->top->str);
        printf("\n\n");
    }
}

// gcc -Wall -std=c11 -pedantic -lm -g -o stack_test stack_test.c ../stack.c
int main(){
    bool return_bool = false;
    char *return_int = NULL;

    stack *stack = malloc(sizeof(stack));
    stackInit(stack);

    // test stackInit
    printf("| stackInit - empty\n");
    print_stack(stack);

    // test stackIsEmpty
    printf("| stackIsEmpty - empty\n");
    return_bool = stackIsEmpty(stack);
    printf("%d\n\n", return_bool);

    // test stackPush
    printf("| stackPush - 1\n");
    return_bool = stackPush(stack, "fsafdsafgfdgdsfdsf");
    printf("%d - ", return_bool);
    print_stack(stack);

    // test stackPush
    printf("| stackPush - 2\n");
    return_bool = stackPush(stack, "bfgvbgbgfdsfgds");
    printf("%d - ", return_bool);
    print_stack(stack);

    // test stackPush
    printf("| stackPush - 9999\n");
    return_bool = stackPush(stack, "9999");
    printf("%d - ", return_bool);
    print_stack(stack);

    // test stackIsEmpty
    printf("| stackIsEmpty - empty\n");
    return_bool = stackIsEmpty(stack);
    printf("%d\n\n", return_bool);

    // test stackTop
    printf("| stackTop - \n");
    return_bool = stackTop(stack, &return_int);
    printf("%d - %s\n\n", return_bool, return_int);
    //free(return_int);

    // test stackPop
    printf("| stackPop - \n");
    return_bool = stackPop(stack, &return_int);
    printf("%d - %s\n\n", return_bool, return_int);
    print_stack(stack);
    free(return_int);
    //return_int = NULL;

    // test stackIsEmpty
    printf("| stackIsEmpty - \n");
    return_bool = stackIsEmpty(stack);
    printf("%d\n\n", return_bool);

    // test stackPop
    printf("| stackPop - \n");
    return_bool = stackPop(stack, &return_int);
    printf("%d - %s\n\n", return_bool, return_int);
    print_stack(stack);
    free(return_int);

    // test stackPop
    printf("| stackPop - \n");
    return_bool = stackPop(stack, &return_int);
    printf("%d - %s\n\n", return_bool, return_int);
    print_stack(stack);
    free(return_int);

    // test stackIsEmpty
    printf("| stackIsEmpty - \n");
    return_bool = stackIsEmpty(stack);
    printf("%d\n\n", return_bool);

    free(stack);

    return 0;
}