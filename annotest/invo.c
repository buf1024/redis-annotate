#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include "../src/config.h"
int main(int argc, char** argv)
{
    printf("invocation_name: %s, invocation_short_name:%s\n",
            program_invocation_name, program_invocation_short_name);

    spt_init(argc, argv);

    setproctitle("hello");
    printf("pid=%ld\n", getpid());
    sleep(100);

    return 0;
}

