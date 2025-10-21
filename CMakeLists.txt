cmake_minimum_required(VERSION 3.10)

project(HeapEngine VERSION 1.0.0 LANGUAGES C)

# --- Configuration ---
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# --- Find Packages ---
# Find the 'Check' unit testing framework
find_package(Check REQUIRED)

# --- Project Settings ---
# Add the 'include' directory to all the targets
include_directories(include)

# --- Compiler Warnings (Good Practice!) ---
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# --- Build Subdirectories ---
# This will build our library
add_subdirectory(src)

# This will build our tests (if enabled)
# TODO: Add an option to enable/disable tests
add_subdirectory(tests)


