cmake_minimum_required(VERSION 3.23)
project(minishell C)

set(CMAKE_C_STANDARD 11)

include_directories(include)
include_directories(src)

add_executable(minishell src/shell.c include/shell.h src/main.c)
