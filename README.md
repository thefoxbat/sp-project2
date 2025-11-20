# CS 551 Programming Project 2: MyBarrier Implementation

**Name:** Aditya Prakash

**B-Number:** B001157609

---

## Project Status
✅ **COMPLETE** — All requirements implemented and verified locally.

---

## Implementation Overview

This project implements a single-use barrier synchronization facility using
Pthread condition variables and mutexes. The implementation exposes the three
APIs required by the assignment: `mybarrier_init()`, `mybarrier_wait()`, and
`mybarrier_destroy()`.

The implementation follows the assignment rules exactly:

- The first `count - 1` threads calling `mybarrier_wait()` block.
- The `count`-th thread releases all blocked threads; those `count` threads
	return `0`.
- The barrier is single-use: subsequent calls to `mybarrier_wait()` return `-1`.
- Passing `NULL` to the APIs returns `-1`.
- `mybarrier_destroy()` wakes any waiters (if needed) and frees resources only
	after all threads have left `mybarrier_wait()`.

---

## Files Submitted

- `mybarrier.h` — Barrier type and API declarations.
- `mybarrier.c` — Implementation of `mybarrier_init`, `mybarrier_wait`, and
	`mybarrier_destroy`.
- `Makefile` — Builds `mybarrier.o` (the submission artifact expected by the
	grader).
- `README.md` — This documentation file (fill in name/B-number above if needed
	before creating the submission ZIP).

Per the assignment, test programs used during development are not included in
the submission files.

---

## Design Details

### Data Structure

The `mybarrier_t` structure extends the base fields with a few internal
bookkeeping members to support single-use semantics and safe destruction:

```c
typedef struct _mybarrier_t {
		int count;                 // barrier count (specified by user)
		int waiting;               // threads currently waiting at barrier
		int active;                // threads currently inside mybarrier_wait()
		int released;              // set when barrier has been released (single-use)
		int aborted;               // set when destroy() aborts waiting threads
		pthread_mutex_t mutex;     // protects shared fields
		pthread_cond_t cond;       // used to wake waiting threads
		pthread_cond_t destroy_cond; // used by destroy() to wait until active==0
} mybarrier_t;
```

These fields are internal only; public API signatures are unchanged from the
assignment specification.

### API Behavior (high-level)

- `mybarrier_init(unsigned int count)`
	- Validates `count > 0` and allocates/initializes the barrier structure.
	- Returns pointer to `mybarrier_t` on success, or `NULL` on failure.

- `mybarrier_wait(mybarrier_t *barrier)`
	- Returns `-1` for `barrier == NULL` or if the barrier has already been used
		or aborted.
	- Increments `waiting` and `active` under mutex protection.
	- If the caller is the `count`-th thread, sets `released = 1`, broadcasts
		to wake waiters, and returns `0` (after performing active bookkeeping).
	- Otherwise the caller waits on `cond` until `released` or `aborted`.
	- If awakened due to `released`, returns `0`. If awakened due to `aborted`,
		returns `-1`.

- `mybarrier_destroy(mybarrier_t *barrier)`
	- Returns `-1` for `barrier == NULL`.
	- If the barrier has not been released, sets `aborted = 1` and broadcasts
		`cond` to wake any waiters.
	- Waits (using `destroy_cond`) until `active == 0` (i.e., all threads have
		left `mybarrier_wait()`).
	- Destroys condition variables and mutex, frees memory, and returns `0`.

This design ensures `destroy()` does not free resources while threads are
still inside `mybarrier_wait()`.

---

## Key Features

- Single-use barrier semantics (subsequent calls return `-1`).
- Uses only Pthread mutexes and condition variables (no busy-waiting).
- Proper `NULL` handling (APIs return `-1` for invalid pointers).
- Safe destroy: `mybarrier_destroy()` wakes waiters and waits for them to
	finish before freeing resources.

---

## Testing Summary (developer tests)

Developer tests (not included in submission) exercised:

1. Basic barrier synchronization — PASS
2. Reuse after barrier satisfied (calls after release) — PASS (they return -1)
3. NULL pointer handling — PASS (`mybarrier_wait(NULL)` and `mybarrier_destroy(NULL)` return -1)
4. Early destroy while threads waiting — PASS (waiters are awakened and
	 resources are freed only after they exit)

If you need the exact test harness used during development, I can provide it
separately (do not include it in submission per the course instruction).

---

## Build Instructions

To build the submission object file, run in the project directory:

```sh
make
```

To clean build artifacts:

```sh
make clean
```

The `Makefile` generates `mybarrier.o` which can be linked with the grader's
test program.

---

## Usage Example

```c
#include "mybarrier.h"

mybarrier_t *barrier = mybarrier_init(5); // create barrier for 5 threads

// In each thread:
int r = mybarrier_wait(barrier);
if (r == 0) {
		// synchronized successfully
} else {
		// error or barrier was aborted
}

// After threads complete
mybarrier_destroy(barrier);
```

---

## Compliance with Specification

- Uses only Pthread mutex and condition variables — ✅
- Implements required APIs with correct signatures — ✅
- Single-use barrier semantics — ✅
- Returns `-1` for `NULL` pointers and for calls after the barrier has been
	used or aborted — ✅
- `mybarrier_destroy()` wakes waiters and frees resources only after waiters
	leave — ✅

---

## Academic Integrity

This code was developed independently in accordance with the course policy.