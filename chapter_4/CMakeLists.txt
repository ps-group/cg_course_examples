project(CG_chapter_4)
cmake_minimum_required(VERSION 2.8)

include(../cmake/SetCompilerWarningAll.cmake)
include(../cmake/win32_paths.cmake)

if(UNIX)
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libs/glm-0.9.8.2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra -Wno-unused-parameter")
    # Сторонние заранее собранные библиотеки
    set(PROJECT_LIBS SDL2 SDL2_image SDL2_ttf GL GLEW GLU boost_filesystem boost_system assimp)
    # Библиотеки, собираемые в составе проекта
    set(PROJECT_LIBS ${PROJECT_LIBS} libsystem libshading libscene libanax libgeometry libcommons)
else(UNIX)
    include_directories(${CG_WIN32_INCLUDE_PATHS})
    link_directories(${CG_WIN32_LINKER_PATHS})
    add_definitions(-DGLEW_STATIC=1)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4201 /wd4324")
    set(CG_GLEW_LIBRARY optimized glew32s debug glew32sd)
    # Сторонние заранее собранные библиотеки
    set(PROJECT_LIBS SDL2main SDL2 SDL2_image SDL2_ttf ${CG_GLEW_LIBRARY} OpenGL32 assimp-vc140-mt)
    # Библиотеки, собираемые в составе проекта
    set(PROJECT_LIBS ${PROJECT_LIBS} libcommons libsystem libgeometry libshading libscene libanax)
endif(UNIX)

# Добавляем 'libs' в пути поиска заголовочных файлов.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs)

# Добавляем библиотеку libcommons с набором сторонних библиотек и ресурсов
file(GLOB SRC_LIBCOMMONS
    "libs/includes/*.hpp"
    "libs/json/*.hpp"
    "libs/tinyxml/*.cpp"
    "libs/tinyxml/*.h"
    "res/*/*.vert"
    "res/*/*.frag"
    "res/*/*.json")
add_library(libcommons ${SRC_LIBCOMMONS})

# Добавляем библиотеку с общим кодом - libsystem.
file(GLOB SRC_LIBSYSTEM
    "libs/libsystem/*.cpp"
    "libs/libsystem/*.h")
add_library(libsystem ${SRC_LIBSYSTEM})

# Добавляем библиотеку с общим кодом - libgeometry.
file(GLOB SRC_LIBGEOMETRY
    "libs/libgeometry/*.cpp"
    "libs/libgeometry/*.h")
add_library(libgeometry ${SRC_LIBGEOMETRY})

# Добавляем библиотеку с общим кодом - libshading.
file(GLOB SRC_LIBSHADING
    "libs/libshading/*.cpp"
    "libs/libshading/*.h")
add_library(libshading ${SRC_LIBSHADING})

# Добавляем библиотеку с общим кодом - libscene.
file(GLOB SRC_LIBSCENE
    "libs/libscene/*.cpp"
    "libs/libscene/*.h")
add_library(libscene ${SRC_LIBSCENE})

# Добавляем стороннюю библиотеку Anax.
file(GLOB SRC_LIBANAX
    "libs/anax/*.cpp"
    "libs/anax/*.hpp"
    "libs/anax/detail/*.cpp"
    "libs/anax/detail/*.hpp"
    "libs/anax/util/*.hpp")
add_library(libanax ${SRC_LIBANAX})

if(MSVC)
target_compile_options(libanax PUBLIC "/wd4100" "/wd4244")
endif(MSVC)

macro(add_lesson NUM)
    file(GLOB SRC_LESSON_${NUM} "lesson_${NUM}/*.cpp" "lesson_${NUM}/*.h")
    add_executable(lesson_${NUM} ${SRC_LESSON_${NUM}})
    target_link_libraries(lesson_${NUM} libcommons libsystem libgeometry libshading libscene libanax ${PROJECT_LIBS})
endmacro(add_lesson)

add_lesson(19)
add_lesson(20)
add_lesson(21)
add_lesson(22)
#add_lesson(23)
#add_lesson(24)

# Выполняем копирование файлов в выходную директорию
file(COPY res DESTINATION ${CMAKE_BINARY_DIR})
