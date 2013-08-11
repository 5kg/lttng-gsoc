#include <stdio.h>
#include <stdlib.h>
#include "syringe.h"

int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr, "Usage: %s PID LIBRARY SYMBOL\n", argv[0]);
        exit(1);
    }

    pid_t pid = strtol(argv[1], NULL, 0);
    char *library = argv[2];

    syringe_dlopen(pid, library);

    return 0;
}
