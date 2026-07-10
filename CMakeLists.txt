cmake_minimum_required(VERSION 3.23)
project(Scribe
    VERSION 0.1.0
    DESCRIPTION "Custom C++20 tokenizer."
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES
    src/word_extracter.cpp
    src/byte_pair_encoder.cpp
)

option(_BUILD_PY_MOD "Build an importable module for Python?" off)

if(SKBUILD OR _BUILD_PY_MOD)
    MESSAGE("Building module for python...")
    find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
    find_package(nanobind CONFIG REQUIRED)
    nanobind_add_module(Scribe ${SOURCES} python/bindings.cpp)

    target_compile_options(Scribe PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
    )

    target_include_directories(Scribe PRIVATE include)

    nanobind_add_stub(Scribe_stub
        MODULE Scribe
        DEPENDS Scribe
        OUTPUT Scribe.pyi
    )

    install(TARGETS Scribe DESTINATION .)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/Scribe.pyi DESTINATION .)
else()
    MESSAGE("Building library for cpp...")
    add_library(Scribe SHARED ${SOURCES})

    target_compile_options(Scribe PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
    )

    target_include_directories(Scribe PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
endif()
