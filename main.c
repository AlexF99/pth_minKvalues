#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_SIZE 120
#define MAX_THREADS 64
#define MAX_TOTAL_ELEMENTS (500 * 1000 * 1000)

float *input;
int nTotalElements = 0;
int k = 0;
int numThreads = 0;

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
    pair_t *vec = malloc(nTotalElmts * sizeof(pair_t));
    for (int i = 0; i < nTotalElmts; i++)
    {
        vec[i].inindex = i;
        vec[i].key = input[i];
    }
    qsort(vec, nTotalElmts, sizeof(pair_t), comparator);
    printf("sorted vec:\n");
    for (int i = 0; i < nTotalElmts; i++)
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

    free(vec);
}

int main(int argc, char const *argv[])
{
    if (argc != 3)
    {
        printf("usage: %s <nTotalElements> <numThreads>\n", argv[0]);
        return 0;
    }
    else
    {
        numThreads = atoi(argv[2]);
        if (numThreads == 0)
        {
            printf("usage: %s <nTotalElements> <numThreads>\n", argv[0]);
            printf("<numThreads> can't be 0\n");
            return 0;
        }
        if (numThreads > MAX_THREADS)
        {
            printf("usage: %s <nTotalElements> <numThreads>\n", argv[0]);
            printf("<numThreads> must be less than %d\n", MAX_THREADS);
            return 0;
        }
        nTotalElements = atoi(argv[1]);
        if (nTotalElements > MAX_TOTAL_ELEMENTS)
        {
            printf("usage: %s <nTotalElements> <numThreads>\n", argv[0]);
            printf("<nTotalElements> must be up to %d\n", MAX_TOTAL_ELEMENTS);
            return 0;
        }
    }

    printf("total elements: %d\n", nTotalElements);
    printf("k: %d\n", k);
    printf("num threads: %d\n", numThreads);

    input = malloc(nTotalElements * sizeof(float));

    for (int i = 0; i < nTotalElements; i++)
    {
        int a = rand();
        int b = rand();
        float v = a * 100.0 + b;
        input[i] = v;
    }

    verifyOutput(input, NULL, nTotalElements, k);

    free(input);
    return 0;
}
