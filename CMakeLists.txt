cmake_minimum_required(VERSION 3.16)
project(imgbuf C)

set(CMAKE_C_STANDARD 11)

find_package(SDL2 REQUIRED)

add_executable(imgbuf main.c)
target_link_libraries(imgbuf SDL2::SDL2)
