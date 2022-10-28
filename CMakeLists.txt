cmake_minimum_required(VERSION 3.23)
project(sallequin C)

set(CMAKE_C_STANDARD 17)

add_executable(
    sallequin
    src/exec.c
    src/exec_builtin.c
    src/main.c
    src/parser.c
    src/shell.c
    src/tokenizer.c
)
target_include_directories(sallequin PRIVATE include)
