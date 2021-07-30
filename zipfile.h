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

size_t findSize(char *file_name)
{
    // opening the file in read mode
    FILE *fp = fopen(file_name, "r");

    // checking if the file exist or not
    if (fp == NULL)
    {
        printf("File Not Found!\n");
        return -1;
    }

    fseek(fp, 0L, SEEK_END);

    // calculating the size of the file
    size_t res = ftell(fp);

    // closing the file
    fclose(fp);

    return res;
}

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

static void write_file(const char *filename, const uint8_t *data, size_t n)
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
char *decode_file(struct MinHeapNode *root, char *s)
{
    char *ans = malloc(20000 * sizeof(char));
    strcpy(ans, "");

    struct MinHeapNode *curr = root;

    for (int i = 0; i < strlen(s); i++)
    {
        if (s[i] == '0')
            curr = curr->left;
        else
            curr = curr->right;

        // reached leaf node
        if (curr->left == NULL && curr->right == NULL)
        {
            //printf("%c\n", curr->data);
            //printf("%c\n", curr->data);
            strcat(ans, &(curr->data));
            curr = root;
        }
    }

    strcat(ans, "\0");
    printf("%s", ans); 
    return ans;
}

#endif