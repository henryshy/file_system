cmake_minimum_required(VERSION 3.16)
project(fs C)

set(CMAKE_C_STANDARD 11)
include_directories(src)
aux_source_directory(src SRC_LIST)
add_executable(fs main.c ${SRC_LIST})
