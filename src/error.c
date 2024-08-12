#include <stdlib.h>
#include <stdio.h>

int panic(const char *msg){
    printf("Erro: %s\n", msg);
    exit(EXIT_FAILURE);
}
