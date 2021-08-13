#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "huffmancoding.h"
#include "zipfile.h"

//Writes the compressed data represented by a binary string as bits to bytes
bool write_binary(char *file_name_dst, char *data, size_t *bytes_written)
{
    FILE *fp = fopen(file_name_dst, "wb");
    size_t number_of_bytes = ceil(strlen(data) / 8.0);
    uint8_t *bytes = (uint8_t *)malloc(number_of_bytes * sizeof(uint8_t));
    memset(bytes, 0, number_of_bytes);

    size_t j = 0;
    for (size_t i = 0; i < strlen(data); i++)
    {
        if (i % 8 == 0 && i != 0)
            j++;
        if (data[i] == '1')
        {
            bytes[j] = bytes[j] | 1U << i % 8;
        }
    }


    *bytes_written = fwrite(bytes, sizeof(size_t), j, fp);

    assert(*bytes_written == j);

    return true;
}

//writes the header, which includes the characters and their frequency, to the zipfile
bool write_header(char *file_name, char *characters, int *frequncies, size_t number_of_characters)
{
    FILE *file_pointer = fopen(file_name, "w");
    fprintf(file_pointer, "%d§", number_of_characters);
    for (int i = 0; i < number_of_characters; i++)
    {
        fprintf(file_pointer, "%c%d", characters[i], frequncies[i]);
    }
    fprintf(file_pointer, "§§");
    fclose(file_pointer);
}

//writes the compressed data to the zipfile
bool create_body(char *data, size_t data_size, char *huffman_code_map[256],
                 size_t *bytes_written, char *output)
{
    size_t buf_cap;
    buf_cap = 4096;
    output = (char *)xmalloc(buf_cap);

    for (size_t i = 0; i < data_size; i++)
    {
        if (buf_cap - *bytes_written < 0)
        {
            buf_cap *= 2;
            output = xrealloc(output, buf_cap);
        }
        uint8_t character = (uint8_t)data[i];
        char *huffman_code = huffman_code_map[character];
        strcat(output, huffman_code);
        *bytes_written += strlen(huffman_code);
    }
    return true;
}

//compress a given file with file_name and writes the compressed data to zip_filename
void huffman_compress(char *zip_filename, char *file_name)
{
    size_t file_size = 0;
    char *buffer = read_file(file_name, &file_size);
    int absolut_frequencies[256];
    memset(absolut_frequencies, 0, 256 * sizeof(int));

    size_t unique_characters = calculate_frequencies(buffer, file_size, absolut_frequencies);

    char *characters = (char *)malloc(unique_characters * sizeof(char));
    int *freqs = (int *)malloc(unique_characters * sizeof(int));

    int j = 0;
    for (int i = 0; i < 256; i++)
    {
        if (absolut_frequencies[i] != 0)
        {
            characters[j] = (char)i;

            freqs[j] = absolut_frequencies[i];

            j++;
        }
    }
    char *huffman_codes[256];
    HuffmanCodes(characters, freqs, unique_characters, huffman_codes);

    //write_header(zip_file, characters, freqs, unique_characters);

    char *binary_string;
    size_t bytes_written = 0;
    create_body(buffer, file_size, huffman_codes, &bytes_written, binary_string);
    printf("got here \n");

    write_binary(zip_filename, binary_string, &bytes_written);

    free(characters);
    free(freqs);
}

bool read_header(FILE *fp, int *file_sz, char *characters, int *frequencies);

bool read_body(FILE *fp, int *file_sz);

//extract data from zip_filename and write decompressed data to filename
void huffman_extract(char *zip_filename, char *filename)
{
    FILE *fp = fopen(zip_filename, "r");

    size_t file_size = 0;
    char *data = read_file(zip_filename, &file_size);
    fclose(fp);

    uint8_t *output = huffman_decoding(root, data, file_size);

    FILE *new_file = fopen(filename, "w");
    fprintf(new_file, "%s", output);

    fclose(new_file);
    free(output);
    free(output);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("usage: zipnow <zipfile> filename\n");
        exit(EXIT_FAILURE);
    }

    char *zip_filename = argv[1];
    char *file_name = argv[2];

    huffman_compress(zip_filename, file_name);

    //huffman_extract(zip_filename, "testtest.txt");

    return 0;
}