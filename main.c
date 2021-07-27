#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define PERROR_IF(cnd, msg) \
    do                      \
    {                       \
        if (cnd)            \
        {                   \
            perror(msg);    \
            exit(1);        \
        }                   \
    } while (0)

// This constant can be avoided by explicitly
// calculating height of Huffman Tree
#define MAX_TREE_HT 100

// A Huffman tree node
struct MinHeapNode
{

    // One of the input characters
    char data;

    // Frequency of the character
    unsigned freq;

    // Left and right child of this node
    struct MinHeapNode *left, *right;
};

// A Min Heap:  Collection of
// min-heap (or Huffman tree) nodes
struct MinHeap
{

    // Current size of min heap
    unsigned size;

    // capacity of min heap
    unsigned capacity;

    // Array of minheap node pointers
    struct MinHeapNode **array;
};

// A utility function allocate a new
// min heap node with given character
// and frequency of the character
struct MinHeapNode *newNode(char data, unsigned freq)
{
    struct MinHeapNode *temp = (struct MinHeapNode *)malloc(
        sizeof(struct MinHeapNode));

    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;

    return temp;
}

// A utility function to create
// a min heap of given capacity
struct MinHeap *createMinHeap(unsigned capacity)

{

    struct MinHeap *minHeap = (struct MinHeap *)malloc(sizeof(struct MinHeap));

    // current size is 0
    minHeap->size = 0;

    minHeap->capacity = capacity;

    minHeap->array = (struct MinHeapNode **)malloc(
        minHeap->capacity * sizeof(struct MinHeapNode *));
    return minHeap;
}

// A utility function to
// swap two min heap nodes
void swapMinHeapNode(struct MinHeapNode **a,
                     struct MinHeapNode **b)

{

    struct MinHeapNode *t = *a;
    *a = *b;
    *b = t;
}

// The standard minHeapify function.
void minHeapify(struct MinHeap *minHeap, int idx)

{

    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx)
    {
        swapMinHeapNode(&minHeap->array[smallest],
                        &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

// A utility function to check
// if size of heap is 1 or not
int isSizeOne(struct MinHeap *minHeap)
{

    return (minHeap->size == 1);
}

// A standard function to extract
// minimum value node from heap
struct MinHeapNode *extractMin(struct MinHeap *minHeap)

{

    struct MinHeapNode *temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];

    --minHeap->size;
    minHeapify(minHeap, 0);

    return temp;
}

// A utility function to insert
// a new node to Min Heap
void insertMinHeap(struct MinHeap *minHeap,
                   struct MinHeapNode *minHeapNode)

{

    ++minHeap->size;
    int i = minHeap->size - 1;

    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq)
    {

        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }

    minHeap->array[i] = minHeapNode;
}

// A standard function to build min heap
void buildMinHeap(struct MinHeap *minHeap)

{

    int n = minHeap->size - 1;
    int i;

    for (i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}

// A utility function to print an array of size n
void insert_element(int arr[], int n, char **char_element)
{
    *char_element = (char *)malloc((n) * sizeof(char));
    int i;
    for (i = 0; i < n; ++i)
    {
        char digit[] = {'0' + arr[i]};
        strcpy(*char_element + i, digit);
    }

    if (n > 0)
        *(*char_element + n) = '\0';
}

// Utility function to check if this node is leaf
int isLeaf(struct MinHeapNode *root)

{

    return !(root->left) && !(root->right);
}

// Creates a min heap of capacity
// equal to size and inserts all character of
// data[] in min heap. Initially size of
// min heap is equal to capacity
struct MinHeap *createAndBuildMinHeap(char data[],
                                      int freq[], int size)

{

    struct MinHeap *minHeap = createMinHeap(size);

    for (int i = 0; i < size; ++i)
        minHeap->array[i] = newNode(data[i], freq[i]);

    minHeap->size = size;
    buildMinHeap(minHeap);

    return minHeap;
}

// The main function that builds Huffman tree
struct MinHeapNode *buildHuffmanTree(char data[],
                                     int freq[], int size)

{
    struct MinHeapNode *left, *right, *top;

    // Step 1: Create a min heap of capacity
    // equal to size.  Initially, there are
    // modes equal to size.
    struct MinHeap *minHeap = createAndBuildMinHeap(data, freq, size);

    // Iterate while size of heap doesn't become 1
    while (!isSizeOne(minHeap))
    {

        // Step 2: Extract the two minimum
        // freq items from min heap
        left = extractMin(minHeap);
        right = extractMin(minHeap);

        // Step 3:  Create a new internal
        // node with frequency equal to the
        // sum of the two nodes frequencies.
        // Make the two extracted node as
        // left and right children of this new node.
        // Add this node to the min heap
        // '$' is a special value for internal nodes, not
        // used
        top = newNode('$', left->freq + right->freq);

        top->left = left;
        top->right = right;

        insertMinHeap(minHeap, top);
    }

    // Step 4: The remaining node is the
    // root node and the tree is complete.
    return extractMin(minHeap);
}

// Prints huffman codes from the root of Huffman Tree.
// It uses arr[] to store codes
void insert_codes(struct MinHeapNode *root, int arr[],
                  int top, char *dst[256])

{

    // Assign 0 to left edge and recur
    if (root->left)
    {

        arr[top] = 0;
        insert_codes(root->left, arr, top + 1, dst);
    }

    // Assign 1 to right edge and recur
    if (root->right)
    {

        arr[top] = 1;
        insert_codes(root->right, arr, top + 1, dst);
    }

    // If this is a leaf node, then
    // it contains one of the input
    // characters, print the character
    // and its code from arr[]
    if (isLeaf(root))
    {

        int current_character_ascii = (int)root->data;
        insert_element(arr, top, &dst[current_character_ascii]);
    }
}

// The main function that builds a
// Huffman Tree and print codes by traversing
// the built Huffman Tree
void HuffmanCodes(char data[], int freq[], int size, char **dst)

{
    // Construct Huffman Tree
    struct MinHeapNode *root = buildHuffmanTree(data, freq, size);

    // Print Huffman codes using
    // the Huffman tree built above
    int arr[MAX_TREE_HT], top = 0;

    insert_codes(root, arr, top, dst);
}

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
    buf = xmalloc(buf_cap);

    *file_sz = 0;
    while (feof(f) == 0)
    {
        if (buf_cap - *file_sz == 0)
        {
            buf_cap *= 2;
            buf = xrealloc(buf, buf_cap);
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
    fprintf(file_pointer,"%d§", number_of_characters); 
    for (int i = 0; i < number_of_characters; i++)
    {
        fprintf(file_pointer, "%c%d", characters[i], frequncies[i]);
    }
    fprintf(file_pointer, "§§");
    fclose(file_pointer); 
}

bool write_body(char *file_name_dst, char* data, uint32_t data_size,  char *huffman_code_map[256]){
    FILE *fp = fopen(file_name_dst, "a"); 
    for(uint32_t i = 0; i < data_size; i++){
        int character = (int) data[i]; 
        fprintf(fp, "%s", huffman_code_map[character]); 
    }
    fclose(fp); 
    return true; 
}

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
    char *no_extension = malloc(file_name_length*sizeof(char)+1);
    strncpy(no_extension, file_name, file_name_length);
    strcat(no_extension, "\0"); 

    return no_extension;
}

bool write_zip(char *file_name)
{

    long int file_size = 0;
    char *buffer = read_file(file_name, &file_size);
     
    file_size = 13;

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

    char * without_extension = get_file_name_no_extension(file_name);
    strcat(without_extension, ".zip");

    write_frequency(without_extension, characters, freqs, unique_characters);

    write_body(without_extension, buffer, file_size, huffman_codes); 
    
    free(characters);
    free(freqs);
    //free(without_extension);
    return true;
}



int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("usage: zipnow filename\n");
        exit(EXIT_FAILURE);
    }
    char *file_name = argv[1];
    
    
    write_zip(file_name);

    
    return 0;
}