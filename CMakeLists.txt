cmake_minimum_required(VERSION 3.5)

project(ggpeg LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE src)

add_executable(ggpeg ${SOURCE}/main.cpp)
