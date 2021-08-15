#ifndef ZIPFILE_H
#define ZIPFILE_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "huffmancoding.h"

#define PERROR_IF(cnd, msg) \
    do                      \
    {                       \
        if (cnd)            \
        {                   \
            perror(msg);    \
            exit(1);        \
        }                   \
    } while (0)

//contains data of the written header
struct header
{
    uint16_t number_of_characters;
    uint8_t *characters;
    uint8_t *frequencies;
};

static void *xmalloc(size_t size)
{
    void *ptr = malloc(size);
    PERROR_IF(ptr == NULL, "malloc");
    return ptr;
}

static void *xrealloc(void *ptr, size_t size)
{
    ptr = realloc(ptr, size);
    PERROR_IF(ptr == NULL, "realloc");
    return ptr;
}

static char *read_file(const char *filename, size_t *file_sz)
{
    FILE *f;
    char *buf;
    size_t buf_cap;

    f = fopen(filename, "r");
    PERROR_IF(f == NULL, "fopen");

    buf_cap = 4096;
    buf = (char *)xmalloc(buf_cap);

    *file_sz = 0;
    while (feof(f) == 0)
    {
        if (buf_cap - *file_sz == 0)
        {
            buf_cap *= 2;
            buf = (char *)xrealloc(buf, buf_cap);
        }

        *file_sz += fread(&buf[*file_sz], sizeof(char), buf_cap - *file_sz, f);
        PERROR_IF(ferror(f), "fread");
    }

    PERROR_IF(fclose(f) != 0, "fclose");

    return buf;
}

static void write_file(const char *filename, const uint8_t *data, size_t n)
{
    FILE *f;

    f = fopen(filename, "w");
    PERROR_IF(f == NULL, "fopen");
    PERROR_IF(fwrite(data, 1, n, f) != n, "fwrite");
    PERROR_IF(fclose(f) != 0, "fclose");
}

#endif