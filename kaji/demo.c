#include <stdlib.h>
#include <stdio.h>

int main()
{
    int i;
    printf("Address of main(): %p\n", main);
    getchar();
    for (i = 0; i < 10; ++i)
        printf("test\n");
    return 0;
}
