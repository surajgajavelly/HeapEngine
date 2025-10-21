# Create a library named 'heap_engine' from the source files
add_library(heap_engine
    my_allocator.c
)

# Link the library to its own public headers
target_include_directories(heap_engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)