cmake_minimum_required(VERSION 3.23)
project(Scribe
    VERSION 0.1.0
    DESCRIPTION "Custom C++20 tokenizer."
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/word_extracter.cpp
    src/byte_pair_encoder.cpp
    python/bindings.cpp
)

option(_BUILD_PY_MOD "Build an importable module for Python?" off)

if(SKBUILD OR _BUILD_PY_MOD)
    find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
    find_package(nanobind CONFIG REQUIRED)
    nanobind_add_module(Scribe ${SOURCES})
    target_include_directories(Scribe PUBLIC include)

    nanobind_add_stub(Scribe_stub
        MODULE Scribe
        OUTPUT Scribe.pyi
    )
else()
    add_library(Scribe SHARED ${SOURCES})
    target_include_directories(Scribe PRIVATE include)
endif()
