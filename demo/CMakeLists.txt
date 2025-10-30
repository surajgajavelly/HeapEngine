add_executable(allocator_demo 
    main.c
)

target_link_libraries(allocator_demo
    PRIVATE
        heap_engine
)