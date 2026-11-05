cmake_minimum_required(VERSION 3.18)
project(hitboxpatch CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(hitboxpatch SHARED src/main.cpp)
target_link_libraries(hitboxpatch PRIVATE log dl)
