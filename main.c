#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "huffmancoding.h"
#include "zipfile.h"

char *get_file_name_no_extension(char *file_name)
{

    size_t file_name_length = 0;

    for (; file_name_length <= strlen(file_name); file_name_length++)
    {
        if (file_name[file_name_length] == '.')
        {
            break;
        }
    }
    char *no_extension = malloc(file_name_length * sizeof(char) + 1);
    strncpy(no_extension, file_name, file_name_length);
    strcat(no_extension, "\0");

    return no_extension;
}

int calculate_frequencies(char *data, long int data_size, int *absolut_frequencies)
{
    int number_of_unique_charaters = 0;

    for (long int i = 0; i < data_size; i++)
    {

        int ascii_value = (int)data[i];
        int temp = absolut_frequencies[ascii_value];
        if (absolut_frequencies[ascii_value] == 0)
            number_of_unique_charaters++;

        absolut_frequencies[ascii_value]++;
    }

    return number_of_unique_charaters;
}

bool write_frequency(char *file_name, char *characters, int *frequncies, size_t number_of_characters)
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

bool write_body(char *file_name_dst, char *data, uint32_t data_size, char *huffman_code_map[256])
{
    FILE *fp = fopen(file_name_dst, "a");
    for (uint32_t i = 0; i < data_size; i++)
    {
        int character = (int)data[i];
        fprintf(fp, "%s", huffman_code_map[character]);
    }
    fclose(fp);
    return true;
}

bool huffman_compress(char *file_name)
{
    long int file_size = 0;
    char *buffer = read_file(file_name, &file_size);

    int absolut_frequencies[256];
    memset(absolut_frequencies, 0L, 256 * sizeof(int));

    int unique_characters = calculate_frequencies(buffer, file_size, absolut_frequencies);

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
    for (int i = 0; i < 256; i++)
    {
        huffman_codes[i] = "$";
    }

    HuffmanCodes(characters, freqs, unique_characters, huffman_codes);

    char *without_extension = get_file_name_no_extension(file_name);
    strcat(without_extension, ".zip");

    write_frequency(without_extension, characters, freqs, unique_characters);

    write_body(without_extension, buffer, file_size, huffman_codes);

    free(characters);
    free(freqs);
    //free(without_extension);
    return true;
}

bool huffman_extract (char *file_name)
{
    FILE *fp = fopen(file_name, "r"); 


}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("usage: zipnow filename\n");
        exit(EXIT_FAILURE);
    }
    char *file_name = argv[1];

    huffman_compress(file_name);

    return 0;
}