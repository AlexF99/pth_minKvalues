#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "heap.h"

#define MAX_SIZE 120
#define MAX_THREADS 64
#define MAX_TOTAL_ELEMENTS (500 * 1000 * 1000)

float *input;
int nTotalElements = 0;
int k = 0;
int numThreads = 0;
pair_t *output;

// pthread variables
pthread_t threads[MAX_THREADS];

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

    // IMPLEMENTAR COMPARACAO

    if (ok)
        printf("\nOutput set verified correctly.\n");
    else
        printf("\nOutput set DID NOT compute correctly!!!\n");

    free(vec);
}

void *thread_routine(void *args)
{
    // thread body
    int thId = *(int *)args;
    sleep(thId);
    printf("now I have started thread number %d\n", thId);
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    if (argc != 4)
    {
        printf("usage: %s <nTotalElements> <k> <numThreads>\n", argv[0]);
        return 0;
    }
    else
    {
        nTotalElements = atoi(argv[1]);
        if (nTotalElements > MAX_TOTAL_ELEMENTS)
        {
            printf("usage: %s <nTotalElements> <k> <numThreads>\n", argv[0]);
            printf("<nTotalElements> must be up to %d\n", MAX_TOTAL_ELEMENTS);
            return 0;
        }
        k = atoi(argv[2]);
        if (k > nTotalElements)
        {
            printf("usage: %s <nTotalElements> <k> <numThreads>\n", argv[0]);
            printf("<k> must be up to %d\n", nTotalElements);
            return 0;
        }
        numThreads = atoi(argv[3]);
        if (numThreads == 0)
        {
            printf("usage: %s <nTotalElements> <k> <numThreads>\n", argv[0]);
            printf("<numThreads> can't be 0\n");
            return 0;
        }
        if (numThreads > MAX_THREADS)
        {
            printf("usage: %s <nTotalElements> <k> <numThreads>\n", argv[0]);
            printf("<numThreads> must be less than %d\n", MAX_THREADS);
            return 0;
        }
    }

    printf("total elements: %d\n", nTotalElements);
    printf("k: %d\n", k);
    printf("num threads: %d\n", numThreads);

    input = malloc(nTotalElements * sizeof(float));
    int *threadIds = malloc(numThreads * sizeof(int));
    output = malloc(k * sizeof(pair_t));

    for (int i = 0; i < nTotalElements; i++)
    {
        int a = rand() % 100; // remove the mod 100's after testing
        int b = rand() % 100;
        float v = a * 100.0 + b;
        input[i] = v;
        if (i < k)
        {
            pair_t elm;
            elm.inindex = i;
            elm.key = v;

            insert(output, elm);
            drawHeapTree(output, (int)(i / 2 + 1));
        }
    }

    threadIds[0] = 0;
    for (int i = 1; i < numThreads; i++)
    {
        threadIds[i] = i;
        pthread_create(&threads[i], NULL, &thread_routine, threadIds + i);
    }
    thread_routine(threadIds); // main has to call it too as it is thread[0]

    for (int i = 1; i < numThreads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    verifyOutput(input, NULL, nTotalElements, k);
    free(input);
    free(threadIds);
    free(output);
    return 0;
}
