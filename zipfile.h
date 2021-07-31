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

static char *read_file(const char *filename, long int *file_sz)
{
    FILE *f;
    char *buf;
    long int buf_cap;

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

static void write_file(const char *filename, const char *data, size_t n)
{
    FILE *f;

    f = fopen(filename, "w");
    PERROR_IF(f == NULL, "fopen");
    PERROR_IF(fwrite(data, 1, n, f) != n, "fwrite");
    PERROR_IF(fclose(f) != 0, "fclose");
}

// function iterates through the encoded string s
// if s[i]=='1' then move to node->right
// if s[i]=='0' then move to node->left
// if leaf node append the node->data to our output string
char *decode_file(struct MinHeapNode *root, char *s, long int file_size)
{
    char *ans = malloc(strlen(s) * sizeof(char));
    strcpy(ans, "");

    struct MinHeapNode *curr = root;

    for (int i = 0; i < file_size; i++)
    {
        if (s[i] == '0')
            curr = curr->left;
        else
            curr = curr->right;

        // reached leaf node
        if (curr->left == NULL && curr->right == NULL)
        {
            strncat(ans, &(curr->data), 1);
            curr = root;
        }
    }

    strcat(ans, "\0");
    return ans;
}

#endif