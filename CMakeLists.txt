cmake_minimum_required(VERSION 3.17.1)

project(granite VERSION 1.0
                DESCRIPTION "Sick Chip8 emulator"
                LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin_release")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin_debug")

add_executable(granite "")
target_compile_features(granite PRIVATE cxx_std_20)

add_subdirectory(src)
