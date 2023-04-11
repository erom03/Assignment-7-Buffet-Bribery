#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Macros for finding parent and children indices
#define PARENT(index) (((index)-1)/2)
#define LEFT(index) (((index)*2)+1)
#define RIGHT(index) (((index)*2)+2)

// Default size of the heap
#define DEFAULT_CAP 8

typedef struct Shipment {
    int arrival;
    int expires;
    int mass;
} Shipment;

// The alias for the type of data stored in the
// heap
typedef Shipment HeapType;

// The struct definition of the heap
// The heap is implemented using an array list
typedef struct Heap {
    HeapType * array;
    int size, cap;
} Heap;

// Prototypes
int higherPriorityThan(HeapType first, HeapType second);
Heap * createHeap();
void deleteHeap(Heap * hp);
void swap(Heap * hp, int index1, int index2);
void percolateUp(Heap * hp, int index);
void percolateDown(Heap * hp, int index);
void append(Heap * hp, HeapType value);
void enqueue(Heap * hp, HeapType value);
void dequeue(Heap * hp);
HeapType front(Heap * hp);
int isEmpty(Heap * hp);
void clamp(Heap * hp, int start, int end);

int main() {
    // Get input for number of shipments
    int numShipments;
    scanf("%d", &numShipments);

    // Create heap to hold shipments
    Heap * shipments = createHeap();

    // Loop for input of each shipment
    for(int i = 0; i < numShipments; i++) {
        // Used to store data to then enqueue
        Shipment temp;

        // Get user input
        scanf("%d %d %d", &temp.arrival, &temp.expires, &temp.mass);

        // Enqueue the shipment to our priority queue
        enqueue(shipments, temp);
    }

    // Vals to track start and end time for eating
    int startEat, endEat;

    // Get users input for start and end eating times
    scanf("%d %d", &startEat, &endEat);

    // Clamp the vals
    clamp(shipments, startEat, endEat);

    for(int i = 0; i < shipments->size; i++) {
        printf("\n%d Arrival: %d\n", i + 1, shipments->array[i].arrival);
        printf("%d Expires: %d\n", i + 1, shipments->array[i].expires);
    }

    // Clean up memory
    deleteHeap(shipments);

    return 0;
}

// Functions

// Clamps vals to start and end times
// Also removes food that expires too soon
void clamp(Heap * hp, int start, int end) {
    // Food is ordered by expiration date
    // Remove all food that expires before start time
    while(front(hp).expires < start)
        dequeue(hp);

    // Set arrivals before start to the start val
    for(int i = 0; i < hp->size; i++) {
        if(hp->array[i].arrival < start)
            hp->array[i].arrival = start;
    }

    // Sets expire times after the end time to the end val
    for(int i = 0; i < hp->size; i++) {
        if(hp->array[i].expires > end)
            hp->array[i].expires = end;
    }
}

// Function to check if the first value is higher priority than the second
// Return 1 if the first is higher priority
// Return 0 otherwise
// This function will be dependent on the type of heap
int higherPriorityThan(HeapType first, HeapType second) {
    if (first.expires < second.expires) return 1;
    return 0;
}

// Function to create a heap
Heap * createHeap() {
    // Allocate
    Heap * res = (Heap *) malloc(sizeof(Heap));

    // Intialize (array list)
    res->size = 0;
    res->cap = DEFAULT_CAP;
    res->array = (HeapType *) malloc(
                 sizeof(HeapType) * res->cap);
    
    // Return
    return res;
}

// Function to delete all the memory in the heap
void deleteHeap(Heap * hp) {
    // TODO: Potentially free the values in the 
    //       heap's array

    // Free the array itself
    free(hp->array);

    // Free the heap
    free(hp);
}

// Function to swap values in the heap
void swap(Heap * hp, int index1, int index2) {
    HeapType tmp = hp->array[index1];
    hp->array[index1] = hp->array[index2];
    hp->array[index2] = tmp;
}

// Function to move values up the heap while higher priority than parent
void percolateUp(Heap * hp, int index) {
    // Loop while the index has a parent
    while (index != 0) {
        // Find the parent
        int parIndex = PARENT(index);

        // Check if we are higher priority than
        // parent
        if (higherPriorityThan(hp->array[index], 
                               hp->array[parIndex])) {
            // Swap to the parent
            swap(hp, index, parIndex);
        
            // The value is now in the parent's location
            index = parIndex;
        } else {
            // The node can stop moving up
            return;
        }
    }
}

// Function to move values down the heap while a child has higher priority
void percolateDown(Heap * hp, int index) {
    int swapped = 0;
    // Loop until there are no more swaps
    do {
        // Set that there are no swaps for the current iteration
        swapped = 0;

        // Track the best index
        int bestIndex = index;

        // Check if a left child is better than best
        int leftIndex = LEFT(index);
        if (leftIndex < hp->size && 
            higherPriorityThan(hp->array[leftIndex], hp->array[bestIndex])) {
            bestIndex = leftIndex;
        }

        // Check if a right child is better than best
        int rightIndex = RIGHT(index);
        if (rightIndex < hp->size && 
            higherPriorityThan(hp->array[rightIndex], hp->array[bestIndex])) {
            bestIndex = rightIndex;
        }

        // Check if a swap is needed
        if (bestIndex != index) {
            swap(hp, bestIndex, index);
            swapped = 1;
            index = bestIndex;
        }
    } while (swapped);
}

// Function to append a value to the end of an array list (heap)
void append(Heap * hp, HeapType value) {
    // Check if the array was full
    if (hp->size == hp->cap) {
        // Expand
        hp->cap *= 2;
        hp->array = (HeapType *) realloc(
                    hp->array, sizeof(HeapType) * hp->cap);
    }
    
    // Add to the end
    hp->array[hp->size] = value;

    // Update size
    hp->size++;
}

// Add a value to the heap
// Insert at end and percolate up
void enqueue(Heap * hp, HeapType value) {
    append(hp, value);
    percolateUp(hp, hp->size - 1);
}

// Function to remove the highest priority element from a heap
// Move last value to the top and percolate down
void dequeue(Heap * hp) {
    swap(hp, 0, hp->size - 1);
    hp->size--;
    percolateDown(hp, 0);
}

// Function to get the highest priority element of the heap (root)
HeapType front(Heap * hp) {
    return hp->array[0];
}

// Function to check if a heap is empty
int isEmpty(Heap * hp) {
    return (hp->size == 0);
}