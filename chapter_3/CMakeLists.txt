project(CG_chapter_3)
cmake_minimum_required(VERSION 2.8)

include(../cmake/SetCompilerWarningAll.cmake)
include(../cmake/win32_paths.cmake)

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra")
    set(PROJECT_LIBS SDL2 SDL2_image SDL2_ttf GL GLEW GLU boost_filesystem boost_system)
else(UNIX)
    include_directories(${CG_WIN32_INCLUDE_PATHS})
    link_directories(${CG_WIN32_LINKER_PATHS})
    add_definitions(-DGLEW_STATIC=1)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4201 /wd4324")
    set(CG_GLEW_LIBRARY optimized glew32s debug glew32sd)
    set(PROJECT_LIBS SDL2main SDL2 SDL2_image SDL2_ttf ${CG_GLEW_LIBRARY} OpenGL32)
endif(UNIX)

macro(add_lesson NUM)
    file(GLOB SRC_LESSON_${NUM} "lesson_${NUM}/*.cpp" "lesson_${NUM}/*.h")
    add_executable(lesson_${NUM} ${SRC_LESSON_${NUM}})
    target_link_libraries(lesson_${NUM} libchapter3 ${PROJECT_LIBS})
endmacro(add_lesson)

file(GLOB SRC_LIBCHAPTER3
    "libchapter3/src/*.cpp"
    "libchapter3/src/*.h"
    "libchapter3/3rd_party/*.cpp"
    "libchapter3/3rd_party/*.h"
    "libchapter3/*.h"
    "res/*.vert"
    "res/*.frag")
add_library(libchapter3 ${SRC_LIBCHAPTER3})
target_include_directories(libchapter3 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libchapter3)

add_lesson(13)
add_lesson(14)
add_lesson(15)
add_lesson(16)
add_lesson(17)
#add_lesson(18)

file(COPY res DESTINATION ${CMAKE_BINARY_DIR})
