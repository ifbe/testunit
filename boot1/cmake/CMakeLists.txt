cmake_minimum_required(VERSION 3.5)
project (hello)
add_executable(hello_exe
        ../src/main.c
)
ADD_LIBRARY(hello_static STATIC
        ../src/main.c
)
SET_TARGET_PROPERTIES(hello_static PROPERTIES OUTPUT_NAME "hello")
ADD_LIBRARY(hello_shared SHARED
        ../src/main.c
)
SET_TARGET_PROPERTIES(hello_shared PROPERTIES OUTPUT_NAME "hello")
