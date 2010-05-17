/*
 * Copyright (c) 2010 Dmitry Goncharov
 *
 * Distributed under the BSD License.
 * (See accompanying file COPYING).
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h> 
#include "g722.h"

/* silence warnings in -ansi -pedantic mode*/
int getopt(int argc, char * const argv[], const char *optstring);
extern char *optarg;
extern int optind;

void print_help()
{
    fprintf(stderr, "usage: g722dec [options]\n\n"
        "reads g722 encoded audio from standard input,\n"
        "decodes it to pcm in the raw format and writes to standard output\n\n"
        "  -8           produce 8000 samples/second output audio\n"
        "               16000 is the default\n"
        "  -b<bitrate>  assume g722 input audio of the specified bitrate\n"
        "               One of 64000, 56000 or 48000. 64000 is the default\n"
        "  -h           this help message\n"
        "  -v           version information\n");
}

int main(int argc, char* const argv[])
{
    int bitrate = 64000;
    int flags = 0;
    int opt;
    while ((opt = getopt(argc, argv, "8b:hv")) != -1)
    {
        switch (opt)
        {
        case '8':
            flags |= G722_SAMPLE_RATE_8000;
            break;
        case 'b':
            bitrate = atoi(optarg);
            if (bitrate != 64000 && bitrate != 56000 && bitrate != 48000)
            {
                fprintf(stderr, "invalid input bitrate specified\n");
                return 1;
            }
            break;
        case 'h':
            print_help();
            return 0;
        case 'v':
        {
            char const version[] = "0.01";
            printf("%s\n", version);
            return 0;
        }
        default:
            print_help();
            return 2;
        }
    }
    if (optind < argc)
    {
        print_help();
        return 3;
    }

    g722_decode_state_t state;
    g722_decode_init(&state, bitrate, flags);
    char inbuf[256 * 1024];
    int16_t outbuf[4 * sizeof inbuf];
    int n; 
    while ((n = read(STDIN_FILENO, inbuf, sizeof inbuf)) > 0)
    {
        int const outlen = g722_decode(&state, outbuf, (const uint8_t*)inbuf, n);
        if (write(STDOUT_FILENO, (char*)outbuf, outlen * sizeof(int16_t)) < outlen * sizeof(int16_t))
        {
            perror("cannot write");
            return 4;
        }
    }
    if (n < 0)
    {
        perror("cannot read");
        return 5;
    }
    return 0;
}

