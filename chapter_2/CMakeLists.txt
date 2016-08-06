project(CG_chapter_2)
cmake_minimum_required(VERSION 2.8)

if(UNIX)
    set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra")
    set(PROJECT_LIBS SDL2 GL GLEW GLU)
else(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4201")
    set(PROJECT_LIBS SDL2main SDL2 glew32 OpenGL32)
endif(UNIX)

include(../cmake/SetCompilerWarningAll.cmake)
include(../cmake/cotire.cmake)

macro(add_lesson NUM)
    aux_source_directory(lesson_${NUM} SRC_LESSON_${NUM})
    add_executable(lesson_${NUM} ${SRC_LESSON_${NUM}})
    target_link_libraries(lesson_${NUM} ${PROJECT_LIBS})
endmacro(add_lesson)

add_lesson(07)
add_lesson(08)
add_lesson(09)
add_lesson(10)
add_lesson(11)
add_lesson(12)

file(COPY res DESTINATION ${CMAKE_BINARY_DIR})
