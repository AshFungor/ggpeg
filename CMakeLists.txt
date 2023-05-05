cmake_minimum_required(VERSION 3.5)

project(ggpeg LANGUAGES CXX VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source directory for this project.
set(source_dir "${CMAKE_SOURCE_DIR}/src")
# Static libraries (modules) to be built.
set(build_dir "${CMAKE_SOURCE_DIR}/build")
set(modules "")
# Unit tests directory.
set(testing_dir "${CMAKE_SOURCE_DIR}/tests")
# Image test files directory.
set(image_test_dir "${testing_dir}/test-data")

set(CMAKE_TOOLCHAIN_FILE "${build_dir}/conan_toolchain.cmake")
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR} ${CMAKE_PREFIX_PATH})

# Catch2 (tests)
find_package(Catch2 REQUIRED)

# spdlog (logging)
find_package(spdlog REQUIRED)

# rang (console colors)
find_package(rang REQUIRED)

# Compession for PNG scanline
find_package(ZLIB REQUIRED)

# Each folder in root source directory is build as a standalone library.
file(GLOB children
    RELATIVE "${source_dir}/"
    CONFIGURE_DEPENDS
    "${source_dir}/*")

# Search trough all entities (folders/files) in source directory.
foreach(child IN ITEMS ${children})
    message(STATUS "Looking through possible target: ${child}")
    set(current "${source_dir}/${child}")
    # Only directories can be represanted as modules.
    if(IS_DIRECTORY ${current})
        message(STATUS "Target <${child}> is a directory, setting library...")
        file(GLOB sources
            CONFIGURE_DEPENDS
            "${current}/*.cpp" "${current}/*.hpp")
        add_library(${child} ${sources})
        list(APPEND modules ${child})
    endif()
endforeach()

include_directories(${source_dir})
include_directories(${rang_INCLUDE_DIR})
include_directories(${Catch2_INCLUDE_DIR})
include_directories(${spdlog_INCLUDE_DIR})
include_directories(${ZLIB_INCLUDE_DIR})

add_executable(ggpeg "${source_dir}/main.cpp")

foreach(lib IN ITEMS ${modules})
    # Link libs to targets
    target_link_libraries(${lib} spdlog::spdlog)
    target_link_libraries(${lib} ZLIB::ZLIB)
    message(STATUS "linking library: ${lib}")
    target_link_libraries(ggpeg PRIVATE ${lib})
    # All tests must be called <lib-target-name>-*-test.cpp
    message(STATUS "making tests for library: ${lib}")
    file(GLOB test_files
        CONFIGURE_DEPENDS
        "${testing_dir}/${lib}-*-test.cpp")
    if(test_files)
        message(STATUS "found tests, making target: ${lib}-test")
        add_executable(${lib}-test ${test_files})
        target_link_libraries(${lib}-test PRIVATE Catch2::Catch2WithMain)
        foreach(module IN ITEMS ${modules})
            target_link_libraries(${lib}-test PRIVATE ${module})
        endforeach()
    else()
        message(STATUS "tests are not found, skipping...")
    endif()
endforeach()

# Search for test data
file(GLOB test_images
    RELATIVE "${image_test_dir}"
    CONFIGURE_DEPENDS
    "${image_test_dir}/*")

# Copy all test data (images) to <resources> directory
foreach(image_file IN ITEMS ${test_images})
    message(STATUS "copying ${image_file} to resources directory...")
    configure_file("${image_test_dir}/${image_file}"
                "${CMAKE_CURRENT_BINARY_DIR}/resources/${image_file}"
                COPYONLY)
endforeach()
