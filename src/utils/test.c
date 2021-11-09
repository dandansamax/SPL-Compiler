#include <stdio.h>
#include <stdlib.h>

int main()
{
    int *ptr = malloc(100);
    printf("%p.%p", ptr, ptr + 1);
    free(ptr);
}