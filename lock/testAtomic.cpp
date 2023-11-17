#include "atomic.h"
#include <stdio.h>

int main(void)
{

    ATOM_INT num1;

    ATOM_INIT(&num1, 0);
    printf("%d\n", ATOM_LOAD(&num1));

    ATOM_FINC(&num1);
    printf("%d\n", ATOM_LOAD(&num1));

    int old = ATOM_LOAD(&num1);
    int nval = old + 1;

    while(!ATOM_CAS(&num1, old, nval))
    {
        printf("%d\n", ATOM_LOAD(&num1));
    }

    printf("%d\n", ATOM_LOAD(&num1));

    return 0;
}