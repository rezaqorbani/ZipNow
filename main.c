#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "huffmancoding.h"
#include "zipfile.h"

//calculate the frequencies of the characters in data, store it in absolut frequencies
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
bool write_body(char *file_name_dst, char *data, uint32_t data_size, char *huffman_code_map[256])
{
    FILE *fp = fopen(file_name_dst, "w");
    for (uint32_t i = 0; i < data_size; i++)
    {
        int character = (int)data[i];
        fprintf(fp, "%s", huffman_code_map[character]);
    }
    fclose(fp);
    return true;
}
//compress a given file with file_name and writes the compressed data to zip_filename
bool huffman_compress(char *zip_filename, char* file_name)
{
    long int file_size = 0;
    char *buffer = read_file(file_name, &file_size);

    int absolut_frequencies[256];
    memset(absolut_frequencies, 0L, 256 * sizeof(int));

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
    for (int i = 0; i < 256; i++)
    {
        huffman_codes[i] = "$";
    }

    HuffmanCodes(characters, freqs, unique_characters, huffman_codes);

    //write_header(zip_file, characters, freqs, unique_characters);

    write_body(zip_filename, buffer, file_size, huffman_codes);

    free(characters);
    free(freqs);

    return true;
}

/*
bool read_header(FILE *fp, int *file_sz, char *characters, int *frequencies)
{
    int buf_cap;
    char *data;

    buf_cap = 4096;
    data = (char *)xmalloc(buf_cap);

    char current_charcter;

    //the first character must be a '§'

    assert((current_charcter = fgetc(fp) )== '§');
    current_charcter = fgetc(fp);
    
    while ((current_charcter = fgetc(fp)) != '§')
    {
        
        *file_sz += fread(&data[*file_sz], sizeof(char), buf_cap - *file_sz, fp);
        PERROR_IF(ferror(fp), "fread");
    }
}
*/
//extract data from zip_filename and write decompressed data to filename
bool huffman_extract(char *zip_filename, char *filename)
{
    FILE *fp = fopen(zip_filename, "r"); 
    
    long int file_size = 0; 
    char* data = read_file(zip_filename, &file_size); 
    fclose(fp);

    char* output = decode_file(root, data, file_size); 


    FILE *new_file = fopen(filename, "w");
    fprintf(new_file, "%s", output); 

    fclose(new_file); 
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("usage: zipnow <zipfile> filename\n");
        exit(EXIT_FAILURE);
    }

    char *zip_filename = argv[1];
    char *file_name = argv[2]; 

    huffman_compress(zip_filename, file_name);

    huffman_extract(zip_filename, "testtest.txt");


    return 0;
}