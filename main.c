#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>

#include "heap.h"
#include "chrono.h"

#define MAX_SIZE 120
#define MAX_THREADS 64
#define MAX_TOTAL_ELEMENTS (500 * 1000 * 1000)

float *input;
int nTotalElements = 0;
int k = 0;
int numThreads = 0;
pair_t *output;
pair_t **heaps;
int inputIndex = 0;
int *heapSizes;

// pthread variables
pthread_t threads[MAX_THREADS];
pthread_barrier_t barrier;

int comparator(const void *e1, const void *e2)
{
    float l = ((pair_t *)e1)->key;
    float r = ((pair_t *)e2)->key;
    return l < r ? -1 : 1;
}

void verifyOutput(const float *input, const pair_t *output, int nTotalElmts, int k)
{
    pair_t *vec = malloc(nTotalElmts * sizeof(pair_t));
    for (int i = 0; i < nTotalElmts; i++)
    {
        vec[i].inindex = i;
        vec[i].key = input[i];
    }
    printf("comparing...\n");
    qsort(vec, nTotalElmts, sizeof(pair_t), comparator);
    qsort((void *)output, k, sizeof(pair_t), comparator);
    // for (int i = 0; i < k; i++)
    // {
    //     printf("key: %.2f, inindex: %d\n", output[i].key, output[i].inindex);
    //     printf("key: %.2f, inindex: %d\n", vec[i].key, vec[i].inindex);
    //     printf("correct: %d\n", vec[i].inindex == output[i].inindex);
    // }

    printf("------------\n");

    int elementsFound = 0;
    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < k; j++)
        {
            if (vec[i].inindex == output[j].inindex)
            {
                elementsFound++;
                break;
            }
        }
    }

    printf("found %d elements\n", elementsFound);

    if (elementsFound < k)
        printf("\nOutput set DID NOT compute correctly!!!\n");
    else
        printf("\nOutput set verified correctly.\n");

    free(vec);
}

int min(int a, int b)
{
    if (a < b)
        return a;
    else
        return b;
}

void *partialDecMax(void *args)
{
    int thId = *(int *)args;
    printf("%d is waiting at barrier\n", thId);
    pthread_barrier_wait(&barrier);
    printf("%d has started working\n", thId);
    int nElements = nTotalElements / numThreads;
    int first = thId * nElements;
    int last = min((thId + 1) * nElements, nTotalElements) - 1;
    if (thId == numThreads - 1)
        last = nTotalElements - 1;

    for (int i = first; i <= last; i++)
    {
        pair_t elm;
        elm.inindex = i;
        elm.key = input[i];
        if (heapSizes[thId] < k)
            insert(heaps[thId], &heapSizes[thId], elm);
        else
            decreaseMax(heaps[thId], k, elm);
    }
    printf("thread %d finished\n", thId);
    if (thId > 0)
        pthread_exit(NULL);
    return NULL;
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
        if (numThreads < 1)
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
    heaps = malloc(numThreads * sizeof(pair_t *));
    heapSizes = malloc(numThreads * sizeof(int));
    for (int i = 0; i < numThreads; i++)
    {
        heapSizes[i] = 0;
        heaps[i] = malloc(k * sizeof(pair_t));
    }

    for (int i = 0; i < nTotalElements; i++)
    {
        int a = rand();
        int b = rand();
        float v = a * 100.0 + b;
        input[i] = v;
    }

    chronometer_t parallelReductionTime;
    chrono_reset(&parallelReductionTime);

    pthread_barrier_init(&barrier, NULL, numThreads);
    for (int i = 1; i < numThreads; i++)
    {
        threadIds[i] = i;
        pthread_create(&threads[i], NULL, &partialDecMax, threadIds + i);
    }
    threadIds[0] = 0;
    chrono_start(&parallelReductionTime);
    partialDecMax(threadIds); // this call will trigger all threads waiting at barrier

    for (int i = 1; i < numThreads; i++)
        pthread_join(threads[i], NULL);

    // joining heaps into one with min k values
    int outputSize = 0;
    for (int i = 0; i < numThreads; i++)
    {
        for (int j = 0; j < k; j++)
        {
            pair_t elm;
            elm.inindex = heaps[i][j].inindex;
            elm.key = heaps[i][j].key;
            if (outputSize < k)
                insert(output, &outputSize, elm);
            else
                decreaseMax(output, outputSize, elm);
        }
    }

    chrono_stop(&parallelReductionTime);
    chrono_reportTime(&parallelReductionTime, "parallelReductionTime");
    double total_time_in_seconds = (double)chrono_gettotal(&parallelReductionTime) / ((double)1000 * 1000 * 1000);
    printf("total_time_in_seconds: %lf s\n", total_time_in_seconds);
    double OPS = (nTotalElements) / total_time_in_seconds;
    printf("Throughput: %lf OP/s\n", OPS);

    verifyOutput(input, output, nTotalElements, k);

    // housekeeping
    free(input);
    free(threadIds);
    free(output);
    for (int i = 0; i < numThreads; i++)
        free(heaps[i]);
    free(heaps);
    free(heapSizes);

    return 0;
}
