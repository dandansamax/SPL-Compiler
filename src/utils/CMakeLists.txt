cmake_minimum_required(VERSION 3.0)
project(SPL C)

set(CMAKE_C_STANDARD 99)
aux_source_directory(./ DIR_SRC)
include_directories(.)
add_executable(SPL main.c ${DIR_SRC})
