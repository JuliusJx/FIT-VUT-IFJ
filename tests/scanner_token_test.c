#include "stdio.h"
#include "string.h"
#include "../scanner.h"
#include "stdlib.h"

/*int main(){
    token *a = GetToken();

    while(a->type != TOKEN_EOF){
        if((a->type == TOKEN_ID) || (a->type == TOKEN_String)){
            printf("[ %s,",a->content.str);
            free(a->content.str);
        }
        else if(a->type == TOKEN_Int)
            printf("[ %d,",a->content.i);
        else if(a->type == TOKEN_Num)
            printf("[ %f,",a->content.f);
        else
            printf("[ ,");
        printf(" TOKEN: %d,",a->type);
        printf(" %d]\n",a->line);
        free(a);
        a = GetToken();
    }

    if((a->type == TOKEN_ID) || (a->type == TOKEN_String)){
        printf("[ %s,",a->content.str);
        free(a->content.str);
    }
    else if(a->type == TOKEN_Int)
        printf("[ %d,",a->content.i);
    else if(a->type == TOKEN_Num)
        printf("[ %f,",a->content.f);
    else
        printf("[ ,");
    printf(" TOKEN: %d,",a->type);
    printf(" %d]\n",a->line);
    free(a);
    return 0;
}*/
