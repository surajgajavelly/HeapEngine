# Create an executable named 'run_tests'
add_executable(run_tests
    test_main.c
)

# Link 'run_tests' against our library and the unity library
target_link_libraries(run_tests
    PRIVATE
        heap_engine
        unity
)