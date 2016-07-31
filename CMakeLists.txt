project(opengl_samples)
cmake_minimum_required(VERSION 2.8)

if(UNIX)
    set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra")
    set(PROJECT_LIBS SDL2 GL GLEW GLU)
else(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4201")
    set(PROJECT_LIBS SDL2main SDL2 glew32 OpenGL32)
endif(UNIX)

include(cmake/SetCompilerWarningAll.cmake)
include(cmake/cotire.cmake)

aux_source_directory(lesson_01 SRC_LESSON_1)
add_executable(lesson_01 ${SRC_LESSON_1})
target_link_libraries(lesson_01 ${PROJECT_LIBS})

aux_source_directory(lesson_02 SRC_LESSON_2)
add_executable(lesson_02 ${SRC_LESSON_2})
target_link_libraries(lesson_02 ${PROJECT_LIBS})

aux_source_directory(lesson_03 SRC_LESSON_3)
add_executable(lesson_03 WIN32 ${SRC_LESSON_3})
target_link_libraries(lesson_03 ${PROJECT_LIBS})

aux_source_directory(lesson_04 SRC_LESSON_4)
add_executable(lesson_04 WIN32 ${SRC_LESSON_4})
target_link_libraries(lesson_04 ${PROJECT_LIBS})

aux_source_directory(lesson_05 SRC_LESSON_5)
add_executable(lesson_05 WIN32 ${SRC_LESSON_5})
target_link_libraries(lesson_05 ${PROJECT_LIBS})

aux_source_directory(lesson_06 SRC_LESSON_6)
add_executable(lesson_06 WIN32 ${SRC_LESSON_6})
target_link_libraries(lesson_06 ${PROJECT_LIBS})

aux_source_directory(lesson_07 SRC_LESSON_7)
add_executable(lesson_07 WIN32 ${SRC_LESSON_7})
target_link_libraries(lesson_07 ${PROJECT_LIBS})

aux_source_directory(lesson_08 SRC_LESSON_8)
add_executable(lesson_08 WIN32 ${SRC_LESSON_8})
target_link_libraries(lesson_08 ${PROJECT_LIBS})

aux_source_directory(lesson_09 SRC_LESSON_9)
add_executable(lesson_09 WIN32 ${SRC_LESSON_9})
target_link_libraries(lesson_09 ${PROJECT_LIBS})

aux_source_directory(lesson_10 SRC_LESSON_10)
add_executable(lesson_10 WIN32 ${SRC_LESSON_10})
target_link_libraries(lesson_10 ${PROJECT_LIBS})

aux_source_directory(lesson_11 SRC_LESSON_11)
add_executable(lesson_11 WIN32 ${SRC_LESSON_11})
target_link_libraries(lesson_11 ${PROJECT_LIBS})

aux_source_directory(lesson_12 SRC_LESSON_12)
add_executable(lesson_12 WIN32 ${SRC_LESSON_12})
target_link_libraries(lesson_12 ${PROJECT_LIBS})

file(COPY res DESTINATION ${CMAKE_BINARY_DIR})
