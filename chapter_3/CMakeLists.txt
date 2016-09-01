project(CG_chapter_3)
cmake_minimum_required(VERSION 2.8)

if(UNIX)
    set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra")
    set(PROJECT_LIBS SDL2 GL GLEW GLU boost_filesystem boost_system)
else(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4201")
    set(PROJECT_LIBS SDL2main SDL2 glew32 OpenGL32)
endif(UNIX)

include(../cmake/SetCompilerWarningAll.cmake)
include(../cmake/cotire.cmake)

macro(add_lesson NUM)
    aux_source_directory(lesson_${NUM} SRC_LESSON_${NUM})
    add_executable(lesson_${NUM} ${SRC_LESSON_${NUM}})
    target_link_libraries(lesson_${NUM} ${PROJECT_LIBS} libchapter3)
endmacro(add_lesson)

aux_source_directory(libchapter3/src SRC_LIBCHAPTER3)
add_library(libchapter3 ${SRC_LIBCHAPTER3})
target_include_directories(libchapter3 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libchapter3)

add_lesson(13)
add_lesson(14)
add_lesson(15)
#add_lesson(16)
#add_lesson(17)
#add_lesson(18)

file(COPY res DESTINATION ${CMAKE_BINARY_DIR})
