cmake_minimum_required(VERSION 3.10)

# set the project name
project(visca27)

# add the library
add_library(visca27 STATIC visca27.cpp)
# include directories
target_include_directories(visca27 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
