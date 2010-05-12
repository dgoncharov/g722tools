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

int main(int argc, char const* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s pcmfile\n", argv[0]);
        return 1;
    }
    int const infd = open(argv[1], O_RDONLY);
    if (infd < 0)
    {
        fprintf(stderr, "cannot open %s, open(): %s\n", argv[1], strerror(errno));
        return 2;
    }
    struct stat st;
    if (fstat(infd, &st) < 0)
    {
        fprintf(stderr, "cannot get file size, fstat(): %s\n", strerror(errno));
        return 3;
    }
    char buf[st.st_size];
    int const n = read(infd, buf, st.st_size);
    if (n < st.st_size)
    {
        fprintf(stderr, "read(): %s\n", strerror(errno));
        return 4;
    }
    uint8_t outbuf[st.st_size];

    g722_encode_state_t state;
    g722_encode_init(&state, 64000, 0);
    int const outlen = g722_encode(&state, outbuf, (const int16_t*) buf, st.st_size / sizeof(int16_t));

    if (write(STDOUT_FILENO, (char*) outbuf, outlen) < outlen)
    {
        fprintf(stderr, "write(): %s\n", strerror(errno));
        return 5;
    }
    return 0;
}

