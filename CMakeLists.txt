cmake_minimum_required(VERSION 3.5)

# Always specify cmake toolchain file.

project(ggpeg LANGUAGES CXX VERSION 1.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source directory for this project.
set(GGPEG_SRC_DIR "${CMAKE_SOURCE_DIR}/src")
# Building directory.
set(GGPEG_BUILD_DIR "${CMAKE_SOURCE_DIR}/build")
# Static libraries (GGPEG_MODULES) to be built.
set(GGPEG_MODULES "")
# Unit tests directory.
set(GGPEG_TEST_DIR "${CMAKE_SOURCE_DIR}/tests")
# Image test files directory.
set(GGPEG_TEST_DATA_DIR "${GGPEG_TEST_DIR}/test-data")
# Include directories.
set(GGPEG_INC_DIRS "${GGPEG_SRC_DIR}")

# Catch2 (tests)
find_package(Catch2 REQUIRED)

# rang (console colors)
find_package(rang REQUIRED)

# Compession for PNG scanline
find_package(ZLIB REQUIRED)

# Each folder in root source directory is build as a standalone library.
file(GLOB found
    CONFIGURE_DEPENDS
    RELATIVE "${GGPEG_SRC_DIR}"
    "${GGPEG_SRC_DIR}/*")

# Search trough all entities (folders/files) in source directory.
foreach(entity IN ITEMS ${found})
    message(STATUS "Looking through possible target: ${child}")
    # Only directories can be represanted as modules.
    set(entity-abs "${GGPEG_SRC_DIR}/${entity}")
    if(IS_DIRECTORY ${entity-abs})
        message(STATUS "Target <${entity}> is a directory, setting library...")
        file(GLOB sources
            CONFIGURE_DEPENDS
            "${entity-abs}/*.cpp" "${entity-abs}/*.hpp")
        add_library(${entity} ${sources})
        # Link libs to targets
        target_link_libraries(${entity} ZLIB::ZLIB)
        target_include_directories(${entity} PRIVATE ${GGPEG_INC_DIRS})
        list(APPEND GGPEG_MODULES ${entity})
    endif()
endforeach()

add_executable(ggpeg "${GGPEG_SRC_DIR}/main.cpp")
target_include_directories(ggpeg PRIVATE ${GGPEG_INC_DIRS})

foreach(lib IN ITEMS ${GGPEG_MODULES})
    message(STATUS "linking library: ${lib}")
    target_link_libraries(ggpeg PRIVATE ${lib})
    # All tests must be called <lib-target-name>-*-test.cpp
    message(STATUS "making tests for library: ${lib}")
    file(GLOB test_files
        CONFIGURE_DEPENDS
        "${GGPEG_TEST_DIR}/${lib}-*-test.cpp")
    if(test_files)
        message(STATUS "found tests, making target: ${lib}-test")
        add_executable(${lib}-test ${test_files})
        target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain)
        target_include_directories(${lib}-test PRIVATE ${GGPEG_INC_DIRS})
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
