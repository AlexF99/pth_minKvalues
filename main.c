#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_SIZE 120

float *input;

typedef struct
{
    float key;
    int inindex;
} pair_t;

int comparator(const void *e1, const void *e2)
{
    float l = ((pair_t *)e1)->key;
    float r = ((pair_t *)e2)->key;
    return l < r ? -1 : 1;
}

void verifyOutput(const float *input, const pair_t *output, int nTotalElmts, int k)
{
    int ok = 1;
    pair_t vec[nTotalElmts];
    for (int i = 0; i < nTotalElmts; i++)
    {
        vec[i].inindex = i;
        vec[i].key = input[i];
    }

    int size = sizeof(vec) / sizeof(pair_t);
    qsort(vec, size, sizeof(pair_t), comparator);

    printf("sorted vec:\n");
    for (int i = 0; i < size; i++)
        printf("key: %f, inindex: %d\n", vec[i].key, vec[i].inindex);

    printf("------------\n");

    // DESCOMENTAR QUANDO TIVER PRONTO
    // for (int i = 0; i < k; i++)
    // {
    //     if (output[i].inindex != vec[i].inindex)
    //     {
    //         ok = 0;
    //         break;
    //     }
    // }

    if (ok)
        printf("\nOutput set verified correctly.\n");
    else
        printf("\nOutput set DID NOT compute correctly!!!\n");
}

int main(int argc, char const *argv[])
{
    int nTotalElements = atoi(argv[1]);
    int k = atoi(argv[2]);
    int numThreads = atoi(argv[3]);
    printf("total elements: %d\n", nTotalElements);
    printf("k: %d\n", k);
    printf("num threads: %d\n", numThreads);

    input = malloc(nTotalElements * sizeof(float));

    int inputSize = 0;
    for (int i = 0; i < nTotalElements; i++)
    {
        int a = rand(); // Returns a pseudo-random integer
        int b = rand(); // same as above
        float v = a * 100.0 + b;
        // inserir o valor v na posição p
        input[inputSize++] = v;
    }

    verifyOutput(input, NULL, nTotalElements, k);

    free(input);
    return 0;
}
