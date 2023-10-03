/*
Programa para achar os k menores valores dado uma entrada de n Floats.
Utiliza numThreads POSIX threads pela biblioteca PThreads.
Autores:
- Alexandre de Oliveira Plugge Freitas
- Wilian Pereira dos Santos

compilar com: gcc -Wall -O3 acharKMenores.c -o acharKMenores -lpthread -lm
*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>

#define MAX_HEAP_SIZE (1024 * 1024)

typedef struct
{
    float key;
    int inindex;
} pair_t;

void drawHeapTree(pair_t *heap, int size, int nLevels)
{
    int offset = 0;
    int nElements = 1;
    printf("heap:\n");
    for (int level = 0; level < nLevels; level++)
    {
        // print all elements in this level
        for (int i = offset; i < size && i < (offset + nElements); i++)
            printf("[%.2lf]", heap[i].key);
        printf("\n");

        offset += nElements;
        nElements *= 2;
    }
    printf("--------end heap---------:\n\n");
}

void swap(int index1, int index2, pair_t *heap)
{
    pair_t temp;
    temp.inindex = heap[index1].inindex;
    temp.key = heap[index1].key;

    heap[index1].inindex = heap[index2].inindex;
    heap[index1].key = heap[index2].key;

    heap[index2].inindex = temp.inindex;
    heap[index2].key = temp.key;
}

void maxHeapify(pair_t *heap, int size, int i)
{
    while (1)
    {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;

        if (left < size && heap[left].key > heap[largest].key)
            largest = left;

        if (right < size && heap[right].key > heap[largest].key)
            largest = right;

        if (largest != i)
        {
            swap(i, largest, heap);
            i = largest;
        }
        else
            break;
    }
}

#define parent(pos) ((pos - 1) / 2)

void heapifyUp(pair_t *heap, int pos)
{
    pair_t temp;
    temp.key = heap[pos].key;
    temp.inindex = heap[pos].inindex;

    while (pos > 0 && temp.key > heap[parent(pos)].key)
    {
        heap[pos].inindex = heap[parent(pos)].inindex;
        heap[pos].key = heap[parent(pos)].key;

        pos = parent(pos);
    }
    heap[pos].inindex = temp.inindex;
    heap[pos].key = temp.key;
}

void insert(pair_t *heap, int *size, pair_t element)
{
    *size += 1;
    int last = *size - 1;

    heap[last].inindex = element.inindex;
    heap[last].key = element.key;
    heapifyUp(heap, last);
}

int isMaxHeap(pair_t *heap, int size)
{
    for (int i = 1; i < size; i++)
        if (heap[i].key <= heap[parent(i)].key)
            continue;
        else
        {
            fprintf(stderr, "isMaxHeap error");
            return 0;
        }
    return 1;
}

void decreaseMax(pair_t *heap, int size, pair_t new_element)
{
    if (size == 0)
        return;

    if (heap[0].key > new_element.key)
    {
        heap[0].key = new_element.key;
        heap[0].inindex = new_element.inindex;
        maxHeapify(heap, size, 0);
    }
}

int isHeapElement(pair_t *heap, int size, pair_t elm)
{
    for (int i = 0; i < size; i++)
    {
        if (elm.inindex == heap[i].inindex)
            return 1;
    }
    return 0;
}

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

    pthread_barrier_init(&barrier, NULL, numThreads);
    for (int i = 1; i < numThreads; i++)
    {
        threadIds[i] = i;
        pthread_create(&threads[i], NULL, &partialDecMax, threadIds + i);
    }
    threadIds[0] = 0;
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

    verifyOutput(input, output, nTotalElements, k);

    // housekeeping
    free(input);
    free(threadIds);
    pthread_barrier_destroy(&barrier);
    free(output);
    for (int i = 0; i < numThreads; i++)
        free(heaps[i]);
    free(heaps);
    free(heapSizes);

    return 0;
}
