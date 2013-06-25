#include <stdio.h>

void foo() {
    printf("I am foo()\n");
    return;
}

void bar() {
    printf("I am bar()\n");
    return;
}

int main()
{
    printf("Hello World!\n");
    for (int i = 0; i < 10; ++i)
        foo();
    return 0;
}
