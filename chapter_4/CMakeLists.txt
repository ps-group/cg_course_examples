project(CG_chapter_4)
cmake_minimum_required(VERSION 2.8)

include(../cmake/SetCompilerWarningAll.cmake)
include(../cmake/win32_paths.cmake)

if(UNIX)
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libs/glm-0.9.8.2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Wno-unused-parameter")
    set(PROJECT_LIBS SDL2 SDL2_image SDL2_ttf GL GLEW GLU boost_filesystem boost_system assimp)
else(UNIX)
    include_directories(${CG_WIN32_INCLUDE_PATHS})
    link_directories(${CG_WIN32_LINKER_PATHS})
    add_definitions(-DGLEW_STATIC=1)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4201 /wd4324")
    set(CG_GLEW_LIBRARY optimized glew32s debug glew32sd)
    set(PROJECT_LIBS SDL2main SDL2 SDL2_image SDL2_ttf ${CG_GLEW_LIBRARY} OpenGL32 assimp)
endif(UNIX)

macro(add_lesson NUM)
    file(GLOB SRC_LESSON_${NUM} "lesson_${NUM}/*.cpp" "lesson_${NUM}/*.h")
    add_executable(lesson_${NUM} ${SRC_LESSON_${NUM}})
    target_link_libraries(lesson_${NUM} libchapter4 libanax ${PROJECT_LIBS})
endmacro(add_lesson)

# Добавляем 'libs' в пути поиска заголовочных файлов.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs)

# Добавляем библиотеку с общим кодом - libchapter4.
file(GLOB SRC_LIBCHAPTER4
    "libs/includes/*.hpp"
    "libs/libscene/*.cpp"
    "libs/libscene/*.h"
    "libs/libsystem/*.cpp"
    "libs/libsystem/*.h"
    "libs/libshading/*.cpp"
    "libs/libshading/*.h"
    "libs/libgeometry/*.cpp"
    "libs/libgeometry/*.h"
    "libs/tinyxml/*.cpp"
    "libs/tinyxml/*.h"
    "libs/libchapter4.h"
    "res/*/*.vert"
    "res/*/*.frag"
    "res/*/*.json")
add_library(libchapter4 ${SRC_LIBCHAPTER4})

# Добавляем библиотеку Anax.
file(GLOB SRC_LIBANAX
    "libs/anax/*.cpp"
    "libs/anax/*.hpp"
    "libs/anax/detail/*.cpp"
    "libs/anax/detail/*.hpp"
    "libs/anax/util/*.hpp")
add_library(libanax ${SRC_LIBANAX})

add_lesson(19)
add_lesson(20)
add_lesson(21)
add_lesson(22)
#add_lesson(23)
#add_lesson(24)

file(COPY res DESTINATION ${CMAKE_BINARY_DIR})
