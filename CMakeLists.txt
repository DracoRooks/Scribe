cmake_minimum_required(VERSION 3.15)
project(Scribe LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. THE CORE LIBRARY (For C++ users)
add_library(scribe_core STATIC 
    src/tokenizer.cpp
)

target_include_directories(scribe_core PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# 2. THE PYTHON MODULE (Only builds when invoked by pip/scikit-build)
if(NOT DEFINED SKBUILD OR SKBUILD)
    find_package(Python COMPONENTS Interpreter Development REQUIRED)
    find_package(nanobind CONFIG REQUIRED)

    nanobind_add_module(Scribe 
        python/bindings.cpp
    )

    target_link_libraries(scribe_tokenizer PRIVATE scribe_core)
endif()
