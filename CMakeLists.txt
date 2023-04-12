cmake_minimum_required(VERSION 3.5)

project(ggpeg LANGUAGES CXX VERSION 0.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(source_dir "${CMAKE_SOURCE_DIR}/src")
set(modules "")

file(GLOB children
    RELATIVE "${source_dir}/"
    CONFIGURE_DEPENDS
    "${source_dir}/*")
foreach(child IN ITEMS ${children})
    message(STATUS "Looking through possible target: ${child}")
    set(current "${source_dir}/${child}")
    if(IS_DIRECTORY ${current})
        message(STATUS "Target <${child}> is a directory, setting library...")
        file(GLOB files
            RELATIVE ${current}
            CONFIGURE_DEPENDS
            "${current}/*.cpp" "${current}/*.hpp")
        set(abs_sources "")
        foreach(file IN ITEMS ${files})
            list(APPEND abs_sources "${current}/${file}")
        endforeach()
        add_library(${child} ${abs_sources})
        list(APPEND modules ${child})
    endif()
endforeach()

include_directories(${source_dir})

add_executable(ggpeg "${source_dir}/main.cpp")

foreach(lib IN ITEMS ${modules})
    message(STATUS "linking library: ${lib}")
    target_link_libraries(ggpeg ${lib})
endforeach()
