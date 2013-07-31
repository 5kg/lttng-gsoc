#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void do_stuff()
{
    int i;
    for (i = 0; i < 10; ++i)
        printf("test\n");
}

int main()
{
    printf("Address of main(): %p\n", main);
    signal(SIGUSR1, do_stuff);
    pause();
    return 0;
}
