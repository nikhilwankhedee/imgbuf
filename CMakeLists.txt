cmake_minimum_required(VERSION 3.10)
project(imgbuf C)

set(CMAKE_C_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

add_executable(imgbuf main.c)

target_include_directories(imgbuf PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(imgbuf PRIVATE ${SDL2_LIBRARIES})
