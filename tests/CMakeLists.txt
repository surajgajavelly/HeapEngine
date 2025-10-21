# Create an executable named 'run_tests'
add_executable(run_tests
    test_main.c
)

# Link our test executable against:
# 1. Our 'heap_engine' library
# 2. The 'Check' unit testing library
target_link_libraries(run_tests
    PRIVATE
        heap_engine
        Check::check
)