typedef struct
{
    float key;
    int inindex;
} pair_t;

void drawHeapTree(pair_t *heap, int size, int nLevels);

void swap(int index1, int index2, pair_t *heap);

void maxHeapify(pair_t *heap, int size, int i);

void heapifyUp(pair_t *heap, int pos);

void insert(pair_t *heap, int *size, pair_t element);

int isMaxHeap(pair_t *heap, int size);

void decreaseMax(pair_t *heap, int size, pair_t new_element);