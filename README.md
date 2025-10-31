# HeapEngine: A Custom C Memory Allocator (V2.1)

This is a custom dynamic memory allocator (`malloc`, `free`, `calloc`, `realloc`)
written in C from scratch. It is built to be portable across standard OS
environments (Linux) and bare-metal embedded systems (like STM32).

The allocator is built using a TDD (Test-Driven Development) workflow,
is fully tested with the Unity framework, and is validated for memory
safety with Valgrind.

## V2.1 Features

* **Configurable Backends:** The heap memory source can be selected at compile time:
    * `HEAP_BACKEND_STATIC`: (Default) Uses a simple static array.
    * `HEAP_BACKEND_SBRK`: Uses `sbrk()` to request the heap from the OS.
    * `HEAP_BACKEND_MMAP`: Uses `mmap()` to request the heap from the OS.
* **STM32 Linker Script Support:** The `STATIC` heap is placed in a `__attribute__((section(".my_heap")))`. This allows a custom linker script (`.ld`) on an STM32 project to place the heap in a specific memory region (e.g., CCM RAM).
* **Docker Environment:** Includes a `Dockerfile` for a 100% reproducible
    build and test environment.
* **Robust CI/CD:** Automated checks via GitHub Actions for formatting, static analysis, building, and memory leak/error checking.

## V1.0 Core Features

* **Explicit Free List:** Manages free memory using a singly-linked list.
* **First-Fit Strategy:** Scans the free list and uses the first block that is large enough.
* **Block Splitting:** Splits large free blocks to reduce internal fragmentation.
* **Forward Coalescing:** `my_free` merges a freed block with the *next*
    physical block if it is also free.
* **Memory Alignment:** `my_malloc` returns memory aligned to **8 bytes**.
* **Robust Safety Checks:**
    * **Magic Numbers (`0xC0FFEE`):** Validates block headers to detect corruption.
    * **Boundary Checks:** `my_free` validates pointers against heap bounds.
    * **Double-Free Protection:** `my_free` detects and ignores attempts to
        free an already-freed block.
* **Full API:** Implements `my_malloc`, `my_free`, `my_calloc`, and `my_realloc`.

## How to Build

This project uses CMake and can be built directly or inside Docker.

### Native Build (Linux)

1.  **Clone the repository:**
    ```bash
    git clone [https://github.com/surajgajavelly/HeapEngine.git](https://github.com/surajgajavelly/HeapEngine.git)
    cd HeapEngine
    ```

2.  **Configure CMake (Select Backend):**
    ```bash
    # To build with the default STATIC backend (for STM32 or general use):
    cmake -S . -B build

    # To build with the SBRK backend:
    cmake -S . -B build -DHEAP_BACKEND=2

    # To build with the MMAP backend:
    cmake -S . -B build -DHEAP_BACKEND=3
    ```

3.  **Build the project:**
    ```bash
    cmake --build build
    ```
This creates `build/tests/run_tests` and `build/demo/allocator_demo`.

### Docker Build (Recommended for CI)

1.  **Build the image:**
    ```bash
    docker build -t heap-engine-dev .
    ```
2.  **Run the build & tests (example using STATIC):**
    ```bash
    docker run --rm -v "$(pwd)":/app heap-engine-dev \
    /bin/bash -c "rm -rf build && \
                  cmake -S . -B build -DHEAP_BACKEND=1 && \
                  cmake --build build && \
                  ./build/tests/run_tests && \
                  valgrind ./build/demo/allocator_demo"
    ```

## How to Test

1.  **Run Unit Tests:**
    ```bash
    ./build/tests/run_tests
    ```

2.  **Run Demo Application (with Valgrind):**
    ```bash
    valgrind --leak-check=full --show-leak-kinds=all ./build/demo/allocator_demo
    ```
A successful run will show `ERROR SUMMARY: 0 errors` and `All heap blocks were freed`.

## Future Work
* **Backward Coalescing:** Implement full two-way coalescing (merging with the *previous* block) by adding footers/boundary tags.
* **Thread-Safety:** Add mutexes to protect the free list for use in an RTOS.