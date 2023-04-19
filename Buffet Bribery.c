#include <stdio.h>
#include <stdlib.h>

// Macros for finding parent and children indices
#define PARENT(index) (((index)-1)/2)
#define LEFT(index) (((index)*2)+1)
#define RIGHT(index) (((index)*2)+2)
#define ITERATIONS 60

// Default size of the heap
#define DEFAULT_CAP 8

typedef struct Shipment {
    int arrival;
    int expires;
    double mass;
} Shipment;

// The alias for the type of data stored in the heap
typedef Shipment HeapType;

// The struct definition of the heap
// The heap is implemented using an array list
typedef struct Heap {
    HeapType * array;
    int size, cap;
} Heap;

// Prototypes
void mergeSort(Shipment * arr, int size);
// Dynamically create an array of the two subarrays merged
Shipment * merge(Shipment * first, int firstSize, Shipment * second, int secondSize);
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
void clamp(Shipment * shipments, int start, int end, int numShipments);
int canDo(Shipment * shipments, int start, int end, int numShipments, double rate);
int update(Heap * arrivedShips, int old, int new, double rate);

int main() {
    // Get input for number of shipments
    int numShipments;
    scanf("%d", &numShipments);

    // Create array to hold shipments
    Shipment * shipments = (Shipment *) malloc (numShipments * sizeof(Shipment));

    // Loop for input of each shipment
    for(int i = 0; i < numShipments; i++) {
        // Get user input
        scanf("%d %d %lf", &shipments[i].arrival, &shipments[i].expires, &shipments[i].mass);
    }

    // Vals to track start and end time for eating
    int startEat, endEat;

    // Get users input for start and end eating times
    scanf("%d %d", &startEat, &endEat);

    // Clamp the vals
    clamp(shipments, startEat, endEat, numShipments);

    // Sort vals by arrival time using merge sort
    mergeSort(shipments, numShipments);

    // Binary Search
    // Assign low value for BS
    double low = 0.0;

    // Create high value for BS
    double high = 0.0;

    // Add mass values to get highest val for BS
    for(int i = 0; i < numShipments; i++) {
        high += shipments[i].mass;
    }

    // Val to store mid point
    double mid;

    // Evaulate optimal eating speed using a BS
    // ITERATIONS times
    for(int i = 0; i < ITERATIONS; i++) {
        mid = (high + low) / 2.0;

        if(canDo(shipments, startEat, endEat, numShipments, mid)) {
            low = mid;
        } else {
            high = mid;
        }
    }

    printf("%.8lf\n", mid);

    // Clean up memory
    free(shipments);

    return 0;
}

// Functions

// Return 1 if the guessed consumption rate works for the given array of shipments
// Return 0 if the guessed consumption rade does not work
int canDo(Shipment * shipments, int start, int end, int numShipments, double rate) {
    // Create a heap
    Heap * arrivedShips = createHeap();

    // Set current time to start time
    int currTime = start;

    // Check if first shipment expires before the start
    int index = 0;
    while(shipments[index].expires < start)
        ++index;
    
    // Add first item
    enqueue(arrivedShips, shipments[index]);

    // Loop through the remaining shipments
    for(int i = index + 1; i < numShipments; i++) {
        // If the next ship is not arriving at the same time as the current ship
        // Update the heap based on the time of the current shipment
        if(currTime != shipments[i].arrival && !update(arrivedShips, currTime, shipments[i].arrival, rate)) {
            // If the update fails
            // Clean memory
            deleteHeap(arrivedShips);    
        
            // Mark as not possible
            return 0;
        } // Else

        // Add current shipment to heap
        enqueue(arrivedShips, shipments[i]);

        // Update time
        currTime = shipments[i].arrival;
    }

    // Update time to last possible time
    // If consumption rate does not work, return 0
    if(currTime != end && !update(arrivedShips, currTime, end, rate)) {
        // Clean memory
        deleteHeap(arrivedShips);    
        
        // Mark as not possible
        return 0;
    }

    // Clean up memory
    deleteHeap(arrivedShips);

    // If we've made it this far, 
    // the consumption rate is valid
    return 1;
}


// Remove the shipments that can be consumed between the given old and new times
// If any shipment expires before finishing consumption, then return 0
// If no shipment is found to expire return 1
int update(Heap * arrivedShips, int oldTime, int newTime, double rate) {
    // Keep track of the current time
    double currTime = (double)oldTime;
    
    // Loop while there is some value in the heap
    while(!isEmpty(arrivedShips)) {
        // Determine the time required to finish consuming the current shipment
        double timeToFinish = front(arrivedShips).mass / rate;

        // Determine the time when the shipment would finish consumption
        double timeFinished = timeToFinish + currTime;
        
        // Check if we cannot finish the shipment before spoiling
        if(timeFinished > front(arrivedShips).expires) {
            // Set current time to expire time
            currTime = front(arrivedShips).expires;

            // Remove expired food from priority queue
            dequeue(arrivedShips);

            // If that was our last value
            // mark the rate as too slow
            if(isEmpty(arrivedShips))
                return 1;
            else    // If not, check next shipment
                continue;
        }

        // Check if we can finish the shipment before the end of the update
        if(timeFinished <= newTime) {
            // Update time
            currTime = timeFinished;

            // Remove from heap
            dequeue(arrivedShips);

            // Continue checking next value
            continue;
        } else {
            // Update the remaining size of the shipment
            arrivedShips->array[0].mass -= (newTime - currTime) * rate;

            // Stop the simulation
            return 1;
        }
    }

    // If we reach here it means that we ran out of food before reaching our new time
    return 0;
}

// Clamps vals to start and end times
// Also removes food that expires too soon
void clamp(Shipment * shipments, int start, int end, int numShipments) {
    // Remove all food that expires before start time
    for(int i = 0; i < numShipments; i++) {
        if(shipments[i].expires < start)
            shipments[i].mass = 0;
    }

    // Set arrivals before start to the start val
    for(int i = 0; i < numShipments; i++) {
        if(shipments[i].arrival < start)
            shipments[i].arrival = start;
    }

    // Sets expire times after the end time to the end val
    for(int i = 0; i < numShipments; i++) {
        if(shipments[i].expires > end)
            shipments[i].expires = end;
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

void mergeSort(Shipment * arr, int size) {
    if (size <= 1) return;

    // Getting the sizes of the two halves
    int sizeLeft = size / 2;
    int sizeRight = size - sizeLeft;

    // Get the pointers into the two halves
    Shipment * leftArr = arr;
    Shipment * rightArr = &arr[sizeLeft];

    // Sort the left side
    mergeSort(leftArr, sizeLeft);

    // Sort the right side
    mergeSort(rightArr, sizeRight);

    Shipment * result = merge(leftArr, sizeLeft, 
                         rightArr, sizeRight);

    // Copy result into arr
    for (int i = 0; i < size; i++)
        arr[i] = result[i];
    
    // Clean up the memory from result
    free(result);
}


Shipment * merge(Shipment * first, int firstSize, Shipment * second, int secondSize) {
    int total = firstSize + secondSize;
    Shipment * result = (Shipment *) calloc(total, sizeof(Shipment));
    
    int firstPtr = 0;
    int secondPtr = 0;
    for (int i = 0; i < total; i++) {
        if (firstPtr == firstSize) {
            // Empty first array
            result[i] = second[secondPtr];
            secondPtr++;
        } else if (secondPtr == secondSize) {
            // Empty second array
            result[i] = first[firstPtr];
            firstPtr++;
        } else if (first[firstPtr].arrival < second[secondPtr].arrival) {
            // First is smaller
            result[i] = first[firstPtr];
            firstPtr++;
        } else if (first[firstPtr].arrival > second[secondPtr].arrival) {
            // Second was smaller
            result[i] = second[secondPtr];
            secondPtr++;
        } else if (first[firstPtr].expires < second[secondPtr].expires) {
            // Both are equal, sort by expiration
            // First expires sooner
            result[i] = first[firstPtr];
            firstPtr++;
        } else {
            // Second expires sooner or at the same time
            result[i] = second[secondPtr];
            secondPtr++;
        }
    }   

    return result;
}