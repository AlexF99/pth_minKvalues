#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>

#include "heap.h"

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
pthread_mutex_t heapLocks[MAX_THREADS];

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
    qsort(vec, nTotalElmts, sizeof(pair_t), comparator);
    qsort(output, k, sizeof(pair_t), comparator);
    printf("sorted vec:\n");
    for (int i = 0; i < k; i++)
    {
        printf("key: %.2f, inindex: %d\n", output[i].key, output[i].inindex);
        printf("key: %.2f, inindex: %d\n", vec[i].key, vec[i].inindex);
        printf("correct: %d\n", vec[i].inindex == output[i].inindex);
    }

    printf("------------\n");

    int elementsFound = 0;
    // ACHA CADA ELEMENTO
    for (int i = 0; i < k; i++)
    {
        int found = 0;
        for (int j = 0; j < k; j++)
        {
            if (vec[i].inindex == output[j].inindex)
            {
                found = 1;
                elementsFound++;
                break;
            }
        }
        if (found == 0)
            break;
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

void *thread_routine(void *args)
{
    // thread body
    int thId = *(int *)args;

    int nElements = nTotalElements / numThreads;

    // assume que temos pelo menos 1 elemento por thread
    int first = thId * nElements;
    int last = min((thId + 1) * nElements, nTotalElements) - 1;

    for (int i = first; i < last; i++)
    {
        printf("thread %d, inputIndex %d, v: %.2f\n", thId, i, input[i]);
        pair_t elm;
        elm.inindex = i;
        elm.key = input[i];
        pthread_mutex_lock(&heapLocks[thId]);
        decreaseMax(heaps[thId], heapSizes[thId], elm);
        pthread_mutex_unlock(&heapLocks[thId]);
    }
    pthread_barrier_wait(&barrier);
    if (thId != 0)
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
    heaps = malloc(numThreads * sizeof(pair_t *));
    heapSizes = malloc(numThreads * sizeof(int));
    for (int i = 0; i < numThreads; i++)
    {
        heapSizes[i] = 0;
        heaps[i] = malloc((int)((k / numThreads) + 2) * sizeof(pair_t));
    }

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
            insert(heaps[i % numThreads], &heapSizes[i % numThreads], elm);
            // printf("inserindo elemento %.2f no heap %d. heapsize: %d\n", elm.key, i % numThreads, heapSizes[i%numThreads]);
        }
    }

    for (int i = 0; i < numThreads; i++)
    {
        printf("heap %d size: %d\n", i, heapSizes[i]);
        pthread_mutex_init(&heapLocks[i], NULL);
    }

    pthread_barrier_init(&barrier, NULL, numThreads);

    threadIds[0] = 0;
    for (int i = 1; i < numThreads; i++)
    {
        threadIds[i] = i;
        pthread_create(&threads[i], NULL, &thread_routine, threadIds + i);
    }
    thread_routine(threadIds); // main has to call it too as it is thread[0]

    // for (int i = 1; i < numThreads; i++)
    //     pthread_join(threads[i], NULL);

    for (int i = 0; i < numThreads; i++)
    {
        drawHeapTree(heaps[i], heapSizes[i], 6);
    }

    int w = 0;
    for (int i = 0; i < numThreads; i++)
    {
        for (int j = 0; j < heapSizes[i]; j++)
        {
            printf("%.2f\n", heaps[i][j].key);
            output[w].inindex = heaps[i][j].inindex;
            output[w++].key = heaps[i][j].key;
        }
        printf("finished thread\n");
    }
    printf("finished, going to verify\n");

    verifyOutput(input, output, nTotalElements, k);

    // housekeeping
    for (int i = 0; i < numThreads; i++)
        pthread_mutex_destroy(&heapLocks[i]);

    free(input);
    free(threadIds);
    free(output);
    // for (int i = 0; i < numThreads; i++)
    //     free(heaps[i]);
    free(heaps);

    return 0;
}
