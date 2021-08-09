#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "huffmancoding.h"
#include "zipfile.h"

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
bool write_body(char *file_name_dst, uint8_t *data, uint32_t data_size, char *huffman_code_map[256])
{
    FILE *fp = fopen(file_name_dst, "w");
    for (uint32_t i = 0; i < data_size; i++)
    {
        int character = data[i];
        memcpy(fp, data, 1); 
    }
    fclose(fp);
    return true;
}

//compress a given file with file_name and writes the compressed data to zip_filename
void huffman_compress(char *zip_filename, char* file_name)
{
    long int file_size = 0;
    uint8_t *buffer = read_file(file_name, &file_size);

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
}


bool read_header(FILE *fp, int *file_sz, char *characters, int *frequencies); 

bool read_body(FILE *fp, int *file_sz);


//extract data from zip_filename and write decompressed data to filename
void huffman_extract(char *zip_filename, char *filename)
{
    FILE *fp = fopen(zip_filename, "r"); 
    
    long int file_size = 0; 
    char* data = read_file(zip_filename, &file_size); 
    fclose(fp);

    uint8_t* output = huffman_decoding(root, data, file_size); 


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