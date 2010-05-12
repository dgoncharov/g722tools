/*
 * Copyright (c) 2010 Dmitry Goncharov
 *
 * Distributed under the BSD License.
 * (See accompanying file COPYING).
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#include <sys/types.h> 
#include <sys/stat.h> 
#include <unistd.h> 
#include <fcntl.h>

#include "g722.h"

/* silence warnings in -ansi -pedantic mode*/
int getopt(int argc, char * const argv[], const char *optstring);
extern int optind;

void print_help()
{
    fprintf(stderr, "g722dec [-evh]\n");
}

int main(int argc, char* argv[])
{
    int flags = 0;
    int opt;
    while ((opt = getopt(argc, argv, "evh")) != -1)
    {
        switch (opt)
        {
        case 'e':
            flags |= G722_SAMPLE_RATE_8000;
            break;
        case 'v':
        {
            char const version[] = "0.01";
            printf("%s\n", version);
            return 0;
        }
        case 'h':
            print_help();
            return 0;
        default:
            print_help();
            return 2;
        }
    }
    if (optind > argc)
    {
        print_help();
        return 3;
    }

    struct stat st;
    if (fstat(STDIN_FILENO, &st) < 0)
    {
        perror("cannot get input file size");
        return 4;
    }
    char buf[st.st_size];
    int const n = read(STDIN_FILENO, buf, st.st_size);
    if (n < st.st_size)
    {
        perror("cannot read");
        return 5;
    }
    int16_t outbuf[st.st_size * 10];

    g722_decode_state_t state;
    g722_decode_init(&state, 64000, flags);
    int const outlen = g722_decode(&state, outbuf, (const uint8_t*)buf, st.st_size);

    if (write(STDOUT_FILENO, (char*)outbuf, outlen * sizeof(int16_t)) < outlen * sizeof(int16_t))
    {
        perror("cannot write");
        return 6;
    }
    return 0;
}

