project(CG_chapter_2)
cmake_minimum_required(VERSION 3.8)

find_package(SDL2 REQUIRED)
find_package(glm REQUIRED)

include(../cmake/SetCompilerWarningAll.cmake)

# Disable configurations `MinSizeRel` and `RelWithDebInfo`
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

macro(add_lesson NUM)
    file(GLOB SRC_LESSON_${NUM} "lesson_${NUM}/*.cpp" "lesson_${NUM}/*.h")
    add_executable(lesson_${NUM} WIN32 ${SRC_LESSON_${NUM}})
    target_link_libraries(lesson_${NUM} PRIVATE libchapter2 SDL2::SDL2 SDL2::SDL2main SDL2_image SDL2_ttf)
	if(UNIX)
		target_compile_options(lesson_${NUM} PRIVATE "-Wall" "-Wextra")
		target_link_libraries(lesson_${NUM} PRIVATE GL GLU)
		target_compile_features(lesson_${NUM} PRIVATE cxx_std_17)
	else(UNIX)
		target_compile_options(lesson_${NUM} PRIVATE "/W4" "/wd4201" "/wd4324")
		target_link_libraries(lesson_${NUM} PRIVATE GLU32 OpenGL32)
	endif(UNIX)
endmacro(add_lesson)

file(GLOB SRC_LIBCHAPTER2 "libchapter2/src/*.cpp"
    "libchapter2/src/*.h"
    "libchapter2/3rd_party/*.cpp"
    "libchapter2/3rd_party/*.h"
    "libchapter2/*.h")
add_library(libchapter2 ${SRC_LIBCHAPTER2})
target_link_libraries(libchapter2 PUBLIC glm)
target_include_directories(libchapter2 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libchapter2)
target_compile_features(libchapter2 PRIVATE cxx_std_17)

add_lesson(07)
add_lesson(08)
add_lesson(09)
add_lesson(10)
add_lesson(11)
add_lesson(12)

file(COPY res DESTINATION ${CMAKE_BINARY_DIR})
