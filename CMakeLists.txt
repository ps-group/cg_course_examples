project(opengl_samples)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra")
set(PROJECT_LIBS SDL2 GL GLEW)

include(cmake/cotire.cmake)

aux_source_directory(lesson_1 SRC_LESSON_1)
add_executable(lesson_1 ${SRC_LESSON_1})
target_link_libraries(lesson_1 ${PROJECT_LIBS})

aux_source_directory(lesson_2 SRC_LESSON_2)
add_executable(lesson_2 ${SRC_LESSON_2})
target_link_libraries(lesson_2 ${PROJECT_LIBS})

aux_source_directory(lesson_3 SRC_LESSON_3)
add_executable(lesson_3 ${SRC_LESSON_3})
target_link_libraries(lesson_3 ${PROJECT_LIBS})

aux_source_directory(lesson_4 SRC_LESSON_4)
add_executable(lesson_4 ${SRC_LESSON_4})
target_link_libraries(lesson_4 ${PROJECT_LIBS})

aux_source_directory(lesson_5 SRC_LESSON_5)
add_executable(lesson_5 ${SRC_LESSON_5})
target_link_libraries(lesson_5 ${PROJECT_LIBS})

aux_source_directory(lesson_6 SRC_LESSON_6)
add_executable(lesson_6 ${SRC_LESSON_6})
target_link_libraries(lesson_6 ${PROJECT_LIBS})

aux_source_directory(lesson_7 SRC_LESSON_7)
add_executable(lesson_7 ${SRC_LESSON_7})
target_link_libraries(lesson_7 ${PROJECT_LIBS})

aux_source_directory(lesson_8 SRC_LESSON_8)
add_executable(lesson_8 ${SRC_LESSON_8})
target_link_libraries(lesson_8 ${PROJECT_LIBS})

aux_source_directory(lesson_9 SRC_LESSON_9)
add_executable(lesson_9 ${SRC_LESSON_9})
target_link_libraries(lesson_9 ${PROJECT_LIBS})
