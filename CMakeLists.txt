cmake_minimum_required(VERSION 3.5)

project(ggpeg LANGUAGES CXX VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source directory for this project.
set(source_dir "${CMAKE_SOURCE_DIR}/src")
# Static libraries (modules) to be built.
set(modules "")
# Unit tests directory.
set(testing_dir "${CMAKE_SOURCE_DIR}/tests")
# Image test files directory.
set(image_test_dir "${testing_dir}/test-data")

# For external libraries
Include(FetchContent)

# Catch2 (tests)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.3.2
)

# spdlog (logging)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog
  GIT_TAG        v1.11.0
)

# rang (console colors)
FetchContent_Declare(
    rang
    GIT_REPOSITORY  https://github.com/agauniyal/rang
    GIT_TAG         v3.2
    # Since library is header-only, build and configure
    # commands are not required.
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
)

# compession for PNG scanline
FetchContent_Declare(
  zlib
  GIT_REPOSITORY https://github.com/madler/zlib
  GIT_TAG        v1.2.13
)

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

FetchContent_MakeAvailable(Catch2 spdlog rang zlib)
include_directories("${rang_SOURCE_DIR}/include")

add_executable(ggpeg "${source_dir}/main.cpp")

foreach(lib IN ITEMS ${modules})
    # link libs to targets
    target_link_libraries(${lib} spdlog)
    target_link_libraries(${lib} zlib)
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
        foreach(_lib IN ITEMS ${modules})
            target_link_libraries(${lib}-test PRIVATE ${_lib})
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
