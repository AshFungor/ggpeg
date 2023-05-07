cmake_minimum_required(VERSION 3.5)

# Always specify cmake toolchain file.

project(ggpeg LANGUAGES CXX VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source directory for this project.
set(GGPEG_SOURCES_DIR "${CMAKE_SOURCE_DIR}/src")
# Building directory.
set(GGPEG_BUILD_DIR "${CMAKE_SOURCE_DIR}/build")
# Static libraries (GGPEG_MODULES) to be built.
set(GGPEG_GGPEG_MODULES "")
# Unit tests directory.
set(GGPEG_TESTS_DIR "${CMAKE_SOURCE_DIR}/tests")
# Image test files directory.
set(GGPEG_TEST_DATA_DIR "${GGPEG_TESTS_DIR}/test-data")
# Include directories.
set(GGPEG_INCLUDE_DIR "${GGPEG_SOURCES_DIR}")

# Catch2 (tests)
find_package(Catch2 REQUIRED)

# rang (console colors)
find_package(rang REQUIRED)

# Compession for PNG scanline
find_package(ZLIB REQUIRED)

# Each folder in root source directory is build as a standalone library.
file(GLOB children
    RELATIVE "${GGPEG_SOURCES_DIR}/"
    CONFIGURE_DEPENDS
    "${GGPEG_SOURCES_DIR}/*")

# Search trough all entities (folders/files) in source directory.
foreach(child IN ITEMS ${children})
    message(STATUS "Looking through possible target: ${child}")
    set(current "${GGPEG_SOURCES_DIR}/${child}")
    # Only directories can be represanted as GGPEG_MODULES.
    if(IS_DIRECTORY ${current})
        message(STATUS "Target <${child}> is a directory, setting library...")
        file(GLOB sources
            CONFIGURE_DEPENDS
            "${current}/*.cpp" "${current}/*.hpp")
        add_library(${child} ${sources})
        # Link libs to targets
        target_link_libraries(${child} ZLIB::ZLIB)
        target_include_directories(${child} PRIVATE ${GGPEG_INCLUDE_DIR})
        list(APPEND GGPEG_MODULES ${child})
    endif()
endforeach()

add_executable(ggpeg "${GGPEG_SOURCES_DIR}/main.cpp")
target_include_directories(ggpeg PRIVATE ${GGPEG_INCLUDE_DIR})

foreach(lib IN ITEMS ${GGPEG_MODULES})
    message(STATUS "linking library: ${lib}")
    target_link_libraries(ggpeg PRIVATE ${lib})
    # All tests must be called <lib-target-name>-*-test.cpp
    message(STATUS "making tests for library: ${lib}")
    file(GLOB test_files
        CONFIGURE_DEPENDS
        "${GGPEG_TESTS_DIR}/${lib}-*-test.cpp")
    if(test_files)
        message(STATUS "found tests, making target: ${lib}-test")
        add_executable(${lib}-test ${test_files})
        target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain)
        target_include_directories(${lib}-test PRIVATE ${GGPEG_INCLUDE_DIR})
        foreach(module IN ITEMS ${GGPEG_MODULES})
            target_link_libraries(${lib}-test PRIVATE ${module})
        endforeach()
    else()
        message(STATUS "tests are not found, skipping...")
    endif()
endforeach()

# Search for test data
file(GLOB test_images
    RELATIVE "${GGPEG_TEST_DATA_DIR}"
    CONFIGURE_DEPENDS
    "${GGPEG_TEST_DATA_DIR}/*")

# Copy all test data (images) to <resources> directory
foreach(image_file IN ITEMS ${test_images})
    message(STATUS "copying ${image_file} to resources directory...")
    configure_file("${GGPEG_TEST_DATA_DIR}/${image_file}"
                "${CMAKE_CURRENT_BINARY_DIR}/resources/${image_file}"
                COPYONLY)
endforeach()
