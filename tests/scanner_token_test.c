#include "stdio.h"
#include "string.h"
#include "../scanner.h"
#include "stdlib.h"

int main(){
    token *a = GetToken();

    while(a->token != TOKEN_EOF){
        if((a->token == TOKEN_ID) || (a->token == TOKEN_String)){
            printf("[ %s,",a->content.str);
            free(a->content.str);
        }
        else if(a->token == TOKEN_Int)
            printf("[ %d,",a->content.i);
        else if(a->token == TOKEN_Num)
            printf("[ %f,",a->content.f);
        else
            printf("[ ,");
        printf(" TOKEN: %d,",a->token);
        printf(" %d]\n",a->line);
        free(a);
        a = GetToken();
    }

    if((a->token == TOKEN_ID) || (a->token == TOKEN_String)){
        printf("[ %s,",a->content.str);
        free(a->content.str);
    }
    else if(a->token == TOKEN_Int)
        printf("[ %d,",a->content.i);
    else if(a->token == TOKEN_Num)
        printf("[ %f,",a->content.f);
    else
        printf("[ ,");
    printf(" TOKEN: %d,",a->token);
    printf(" %d]\n",a->line);
    free(a);
    return 0;
}
