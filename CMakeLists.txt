cmake_minimum_required(VERSION 3.10)
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/common/bootstrap.cmake" NO_POLICY_SCOPE)
# set the project name
project(visca27)
include(compilerconfig)
include(defaults)
include(helpers)
# add the librarys
add_library(visca27 STATIC visca27.cpp ViscaAPI.cpp)
if(OS_LINUX
    OR OS_FREEBSD
    OR OS_OPENBSD)
# add fPIC on Linux to prevent shared object errors
set_property(TARGET visca27 PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()
# include directories
target_include_directories(visca27 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
