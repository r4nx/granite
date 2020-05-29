cmake_minimum_required(VERSION 3.17.1)

project(granite VERSION 1.0
                DESCRIPTION "Sick Chip8 emulator"
                LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin_release")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin_debug")

# Settings

# [!] CAUTION [!]
# If you want this to work, you should also link SFML static libraries with
# appropriate runtime library (those precompiled libraries from official
# distribution are not suitable, at least for now).
set(STATIC_RUNTIME_LINKAGE OFF)

# Third party
set(SFML_STATIC_LIBRARIES true)
find_package(SFML COMPONENTS window graphics)
if (NOT SFML_FOUND)
    message(FATAL_ERROR "SFML was not found")
endif()

if (WIN32)
    set(GRANITE_SUBSYSTEM WIN32)
else()
    set(GRANITE_SUBSYSTEM "")
endif()

# Actual executable
add_executable(granite "${GRANITE_SUBSYSTEM}")
target_compile_features(granite PRIVATE cxx_std_20)
target_link_libraries(granite PRIVATE sfml-window sfml-graphics)
if(STATIC_RUNTIME_LINKAGE)
    set_target_properties(granite PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()

add_subdirectory(src)
