#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void) {
    int *p = malloc(sizeof(int));
    int *q = p;
    free(p);
    *q = 11;
    printf("%p", q);
    return 0;
}
