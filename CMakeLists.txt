
CMAKE_MINIMUM_REQUIRED(VERSION 3.0 FATAL_ERROR)
PROJECT(baselib)
option(TEST "ON for complile test" ON) 

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -export-dynamic -O0 -ggdb -pg -std=c++17 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")

include_directories(.)

set(LIB_SRC 
    baselib/CentralCache.cc
    baselib/PageCache.cc
    baselib/Common.cc
    baselib/ThreadCache.cc
    )  

add_library(baselib SHARED ${LIB_SRC})  # (库文件名称 STATIC 文件)

set(LIB_LIB
baselib)

add_executable(Benchmark src/Benchmark.cc)       # (可执行文件名称 文件)
target_link_libraries(Benchmark ${LIB_LIB})  # (库文件名称/可执行文件名称 链接的库文件名称)

add_executable(UnitTest src/UnitTest.cc)       # (可执行文件名称 文件)
target_link_libraries(UnitTest ${LIB_LIB})  # (库文件名称/可执行文件名称 链接的库文件名称)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)


