cmake_minimum_required(VERSION 3.10)

# set the project name
project(visca27)

# add the library
add_library(visca27 STATIC visca27.cpp ViscaAPI.cpp)
# include directories
target_include_directories(visca27 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.14" CACHE STRING "Minimum OS X deployment version")
    include_directories(SYSTEM ${CMAKE_OSX_SYSROOT}/usr/include)
endif()
# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
