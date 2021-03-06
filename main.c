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
bool write_bits(char *file_name, char *data, size_t *data_size, size_t *bytes_written)
{
    FILE *fp = fopen(file_name, "wb");
    uint8_t *bytes = (uint8_t *)xmalloc(*data_size * sizeof(uint8_t));
    memset(bytes, 0, *data_size);

    size_t j = 0;
    for (size_t i = 0; i < *data_size; i++)
    {
        if (i % 8 == 0 && i != 0)
            j++;
        if (data[i] == '1')
        {
            bytes[j] = bytes[j] | 1U << (i % 8);
        }
    }
    *bytes_written = fwrite(bytes, sizeof(uint8_t), j, fp);
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
static char *create_body(char *data, size_t data_size, char *huffman_code_map[256],
                         size_t *bits_written)
{
    size_t buf_cap;
    char *output;

    buf_cap = 4096;
    output = xmalloc(buf_cap);
    output[0] = '\0'; //make output null-terminated

    *bits_written = 0; 
    for (size_t i = 0; i < data_size; i++)
    {
        uint8_t character = (uint8_t)data[i];
        char *huffman_code = huffman_code_map[character];
        for (size_t j = 0; j < strlen(huffman_code); j++)
        {
            if (buf_cap - *bits_written <= 0)
            {
                buf_cap *= 2;
                output = xrealloc(output, buf_cap);
            }
            strncat(output, &huffman_code[j], 1);
            (*bits_written)++;
        }
    }
    return output;
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

    size_t data_size = 0;
    char *binary_string = create_body(buffer, file_size, huffman_codes, &data_size);
    size_t bits_written;
    write_bits(zip_filename, binary_string, &data_size, &bits_written);

    free(binary_string);
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