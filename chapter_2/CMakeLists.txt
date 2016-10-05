project(CG_chapter_2)
cmake_minimum_required(VERSION 2.8)

include(../cmake/SetCompilerWarningAll.cmake)
include(../cmake/PrecompiledHeader.cmake)
include(../cmake/win32_paths.cmake)

# Disable configurations `MinSizeRel` and `RelWithDebInfo`
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

if(UNIX)
    set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra")
    set(PROJECT_LIBS SDL2 SDL2_image SDL2_ttf GL GLU boost_filesystem boost_system)
else(UNIX)
    include_directories(${CG_WIN32_INCLUDE_PATHS})
    link_directories(${CG_WIN32_LINKER_PATHS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4201 /wd4324")
    set(PROJECT_LIBS SDL2main SDL2 SDL2_image SDL2_ttf GLU32 OpenGL32)
endif(UNIX)

macro(add_lesson NUM)
    file(GLOB SRC_LESSON_${NUM} "lesson_${NUM}/*.cpp" "lesson_${NUM}/*.h")
    add_executable(lesson_${NUM} ${SRC_LESSON_${NUM}})
    target_link_libraries(lesson_${NUM} ${PROJECT_LIBS} libchapter2)
    add_precompiled_header(lesson_${NUM} "stdafx.h")
endmacro(add_lesson)

file(GLOB SRC_LIBCHAPTER2 "libchapter2/src/*.cpp"
    "libchapter2/src/*.h"
    "libchapter2/3rd_party/*.cpp"
    "libchapter2/3rd_party/*.h"
    "libchapter2/*.h")
add_library(libchapter2 ${SRC_LIBCHAPTER2})
target_include_directories(libchapter2 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libchapter2)

add_lesson(07)
add_lesson(08)
add_lesson(09)
add_lesson(10)
add_lesson(11)
add_lesson(12)

file(COPY res DESTINATION ${CMAKE_BINARY_DIR})
