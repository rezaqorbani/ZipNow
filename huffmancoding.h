#ifndef HUFFMANCODING_H
#define HUFFMANCODING_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// This constant can be avoided by explicitly
// calculating height of Huffman Tree
#define MAX_TREE_HT 100

struct MinHeapNode *root;

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
void insert_element(int arr[], int n, char **dst)
{
    *dst = (char *)malloc((n) * sizeof(char));
    int i;
    for (i = 0; i < n; ++i)
    {
        char digit[] = {'0' + arr[i]};
        strcpy(*dst + i, digit);
    }

    if (n > 0)
        *(*dst + n) = '\0';
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

    //struct MinHeapNode *root = buildHuffmanTree(data, freq, size);
    root = buildHuffmanTree(data, freq, size);

    // Print Huffman codes using
    // the Huffman tree built above
    int arr[MAX_TREE_HT], top = 0;

    insert_codes(root, arr, top, dst);
}

//calculate the frequencies of the characters in data, store it in absolut frequencies
int calculate_frequencies(char *data, long int data_size, int *absolut_frequencies)
{
    int number_of_unique_charaters = 0;

    for (long int i = 0; i < data_size; i++)
    {
        uint8_t ascii_value = data[i];
        if (absolut_frequencies[ascii_value] == 0)
            number_of_unique_charaters++;

        absolut_frequencies[ascii_value]++;
    }
    return number_of_unique_charaters;
}

// function iterates through the encoded string s
// if s[i]=='1' then move to node->right
// if s[i]=='0' then move to node->left
// if leaf node append the node->data to our output string
uint8_t *huffman_decoding(struct MinHeapNode *root, uint8_t *src, size_t src_size)
{
    uint8_t *decoded = (uint8_t*) malloc(src_size * sizeof(uint8_t));
    

    struct MinHeapNode *curr = root;

    for (int i = 0; i < src_size; i++)
    {
        if (src[i] == '0')
            curr = curr->left;
        else
            curr = curr->right;

        // reached leaf node
        if (curr->left == NULL && curr->right == NULL)
        {
            memcpy(decoded, &(curr->data), sizeof(uint8_t));
            curr = root;
        }
    }

    return decoded;
}

#endif