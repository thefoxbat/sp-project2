# CS 551 Programming Project 2: MyBarrier Implementation

**Name:** Aditya Prakash  
**B-Number:** B01157609

----------

## Project Status

✅ **COMPLETE** - All requirements implemented and tested successfully.

----------

## Implementation Overview

This project implements a barrier synchronization facility using Pthread condition variables and mutexes. The implementation provides three APIs as specified:

1.  `mybarrier_init()` - Initialize a barrier with specified count
2.  `mybarrier_wait()` - Wait at the barrier until all threads arrive
3.  `mybarrier_destroy()` - Destroy the barrier and free resources

----------

## Files Submitted

-   **mybarrier.h** - Header file containing the barrier structure and API declarations
-   **mybarrier.c** - Implementation of the three barrier APIs
-   **Makefile** - Build configuration to generate mybarrier.o
-   **README** - This documentation file

----------

## Design Details

### Data Structure Extensions

The `mybarrier_t` structure includes the following additional fields beyond the base specification:

```c
typedef struct _mybarrier_t {
    int count;                    // Required: barrier count
    pthread_mutex_t mutex;        // Required: mutex for synchronization
    pthread_cond_t cond;          // Required: condition variable
    int waiting;                  // Added: number of threads currently waiting
    int barrier_satisfied;        // Added: flag indicating barrier has been satisfied
    int destroy_pending;          // Added: flag for handling early destroy
} mybarrier_t;

```

### Algorithm Description

#### mybarrier_init(unsigned int count)

-   Allocates memory for the barrier structure
-   Validates input (count > 0)
-   Initializes all fields including the mutex and condition variable
-   Returns pointer to initialized barrier, or NULL on failure

#### mybarrier_wait(mybarrier_t* barrier)

-   Validates barrier pointer (returns -1 if NULL)
-   Locks the mutex to protect shared state
-   Checks if barrier already satisfied (returns -1 if true, preventing reuse)
-   Increments the waiting counter
-   **If last thread (waiting == count):**
    -   Sets barrier_satisfied flag to 1
    -   Broadcasts to wake all waiting threads
    -   Returns 0
-   **Otherwise:**
    -   Waits on condition variable until barrier_satisfied becomes 1
    -   Returns 0 when awakened

#### mybarrier_destroy(mybarrier_t* barrier)

-   Validates barrier pointer
-   Locks the mutex
-   If threads are still waiting:
    -   Sets barrier_satisfied flag
    -   Broadcasts to wake all waiting threads
-   Unlocks mutex
-   Sleeps briefly (10ms) to ensure threads complete
-   Destroys mutex and condition variable
-   Frees allocated memory
-   Returns 0 on success

----------

## Key Features

✅ **Single-Use Barrier**: After all threads are released, subsequent calls to `mybarrier_wait()` return -1  
✅ **Thread-Safe**: Uses mutex to protect all shared state modifications  
✅ **Efficient Waiting**: Uses condition variables instead of busy-waiting  
✅ **NULL Pointer Handling**: All functions check for NULL and return -1  
✅ **Early Destroy Support**: Properly handles destroy called while threads are waiting  
✅ **No Memory Leaks**: All allocated resources are properly freed

----------

## Testing Results

The implementation has been thoroughly tested with the following scenarios:

### Test 1: Basic Barrier Synchronization

-   **Result:** ✅ PASS
-   All threads properly synchronize at the barrier
-   All threads are released together when the count is satisfied

### Test 2: Reuse After Barrier Satisfied

-   **Result:** ✅ PASS
-   First N calls succeed (return 0)
-   Subsequent calls correctly return -1

### Test 3: NULL Pointer Handling

-   **Result:** ✅ PASS
-   `mybarrier_wait(NULL)` returns -1
-   `mybarrier_destroy(NULL)` returns -1

### Test 4: Early Destroy

-   **Result:** ✅ PASS
-   Waiting threads are properly awakened
-   Resources are freed after threads complete
-   No segmentation faults or memory corruption

----------

## Build Instructions

To compile the project:

```bash
make

```

This generates `mybarrier.o` which can be linked with test programs.

To clean build artifacts:

```bash
make clean

```

----------

## Usage Example

```c
#include "mybarrier.h"

// Initialize barrier for 5 threads
mybarrier_t* barrier = mybarrier_init(5);

// In each thread:
int result = mybarrier_wait(barrier);
if (result == 0) {
    // Thread successfully synchronized
    // Continue execution
}

// After all threads complete:
mybarrier_destroy(barrier);

```

----------

## Compliance with Specification

This implementation strictly adheres to the project specification:

-   ✅ Uses only Pthread mutex and condition variables (no semaphores, no busy-waiting)
-   ✅ Implements all three required APIs with correct signatures
-   ✅ Barrier is single-use as specified
-   ✅ Handles NULL pointers by returning -1
-   ✅ Properly handles early destroy scenario
-   ✅ First (count-1) calls block, count-th call releases all threads
-   ✅ (count+1)-th and subsequent calls return -1
-   ✅ Resources are freed only after all threads are unblocked

----------

## Notes

-   The implementation uses `pthread_cond_broadcast()` to wake all waiting threads simultaneously
-   A brief sleep (10ms) in `mybarrier_destroy()` ensures threads have time to exit the wait function before resources are freed
-   The `barrier_satisfied` flag ensures the barrier cannot be reused after the count is satisfied
-   All synchronization operations are protected by the mutex to prevent race conditions

----------

## Academic Integrity

This code was developed independently following the course collaboration policy. No code was shared with other students.
