cmake_minimum_required(VERSION 3.10)

project(HeapEngine VERSION 1.0.0 LANGUAGES C)

# --- Configuration ---
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# # --- Project Settings ---
# # Add the 'include' directory to all the targets
# include_directories(include)

# --- Compiler Warnings (Good Practice!) ---
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -g)
    add_compile_definitions(UNITY_OUTPUT_COLOR)
endif()

# --- Build our 3rd-party Unity library ---
# This creates a library target named 'unity'
add_library(unity
    vendor/unity/unity.c
)

# This tells any target linking 'unity' to also include its directory
target_include_directories(unity
    PUBLIC
        vendor/unity
)

# --- Build Subdirectories ---
# This will build our library
add_subdirectory(src)

# This will build our tests (if enabled)
# TODO: Add an option to enable/disable tests
add_subdirectory(tests)

# This will build our demo
add_subdirectory(demo)


