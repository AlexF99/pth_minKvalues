#include <stdio.h>
#include <math.h>
#include "heap.h"

#define MAX_HEAP_SIZE (1024 * 1024)
int size = 0;

void drawHeapTree(pair_t *heap, int nLevels)
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

void maxHeapify(pair_t *heap, int i)
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

void insert(pair_t *heap, pair_t element)
{
    size += 1;
    int last = size - 1;

    heap[last].inindex = element.inindex;
    heap[last].key = element.key;
    heapifyUp(heap, last);
}

int isMaxHeap(pair_t *heap)
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

void decreaseMax(pair_t *heap, pair_t new_element)
{
    if (size == 0)
        return;

    if (heap[0].key > new_element.key)
    {
        heap[0].key = new_element.key;
        heap[0].inindex = new_element.inindex;
        maxHeapify(heap, 0);
    }
}