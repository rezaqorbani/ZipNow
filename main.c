#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

bool lz77_compress(const uint8_t *src, size_t src_len, size_t max_dist,
                   size_t max_len, bool allow_overlap,
                   bool (*lit_callback)(uint8_t lit, void *aux),
                   bool (*backref_callback)(size_t dist, size_t len, void *aux),
                   void *aux);

int main()
{
    size_t sizeArray[] = {1, 2, 3, 4, 5};

    for (int i = 0; i < 5; i++)
    {

        printf("%d\n", sizeArray[i]);
    }

    return 0;
}