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
    foo();
    return 0;
}
