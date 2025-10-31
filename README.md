# HeapEngine: A Custom C Memory Allocator (V1.0)

This is a custom dynamic memory allocator written in C from scratch. It implements the standard C library allocation functions (`malloc`, `free`, `calloc`, `realloc`) using an explicit free list and a first-fit strategy.

This project was built to demonstrate core C programming, low-level memory management, and professional software development practices, including Test-Driven Development (TDD), CI/CD, and robust error handling.

## V1.0 Features

* **Explicit Free List:** Manages free memory using a singly-linked list.
* **First-Fit Strategy:** Scans the free list and uses the first block that is large enough for an allocation.
* **Block Splitting:** If a found free block is larger than required, it is split into an allocated block and a new, smaller free block to reduce internal fragmentation.
* **Forward Coalescing:** `my_free` automatically merges a freed block with the *next* physical block if it is also free, reducing external fragmentation.
* **Memory Alignment:** `my_malloc` returns memory aligned to **8 bytes**. This is achieved by storing an offset just before the user's data pointer.
* **Robust Safety Checks:**
    * **Magic Numbers:** Uses `0xC0FFEE` in block headers to detect heap corruption and invalid pointers.
    * **Boundary Checks:** `my_free` validates that pointers are within the heap bounds.
    * **Double-Free Protection:** `my_free` detects and ignores attempts to free an already-freed block.
* **Full API:** Implements `my_malloc`, `my_free`, `my_calloc`, and `my_realloc`.
* **Testing:** Includes a full unit test suite (18 tests) using the **Unity** framework.
* **CI/CD:** Automated builds, formatting (`clang-format`), static analysis (`cppcheck`), and Valgrind leak checking via **GitHub Actions**.

## How to Build

This project uses CMake.

1.  **Clone the repository:**
    ```bash
    git clone [https://github.com/surajgajavelly/HeapEngine.git](https://github.com/surajgajavelly/HeapEngine.git)
    cd HeapEngine
    ```

2.  **Configure CMake:**
    ```bash
    cmake -S . -B build
    ```

3.  **Build the project:**
    ```bash
    cmake --build build
    ```
    This will create two executables:
    * `build/tests/run_tests`
    * `build/demo/allocator_demo`

## How to Test

1.  **Run Unit Tests:**
    ```bash
    ./build/tests/run_tests
    ```

2.  **Run Demo Application (with Valgrind for leak checking):**
    ```bash
    valgrind --leak-check=full --show-leak-kinds=all ./build/demo/allocator_demo
    ```
    A successful run will show `ERROR SUMMARY: 0 errors` and `All heap blocks were freed`.

## Future Work (V2.0 Ideas)

* **Configurable Backends:** Add compile-time flags to allow the heap to be sourced from `sbrk()` or `mmap()` on OS-level environments.
* **Backward Coalescing:** Implement full two-way coalescing (merging with the *previous* block) by adding footers/boundary tags.
* **Thread-Safety:** Add mutexes to protect the free list, making the allocator safe for use in an RTOS or multi-threaded application.
* **Optimizations:** Explore more advanced free list strategies (like segregated lists) to improve performance.