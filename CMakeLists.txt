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

# Catch2 path.
set(Catch2_DIR "external/Catch2/lib/cmake/Catch2")

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

# Catch2 (for unit testing)
find_package(Catch2 3 REQUIRED CONFIG)

add_executable(ggpeg "${source_dir}/main.cpp")

foreach(lib IN ITEMS ${modules})
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
        target_link_libraries(${lib}-test PRIVATE ${lib})
    else()
        message(STATUS "tests are not found, skipping...")
    endif()
endforeach()
