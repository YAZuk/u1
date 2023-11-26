cmake_minimum_required(VERSION 3.18)
project(u2)

set(CMAKE_CXX_STANDARD 11)
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(Threads REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )

add_executable(u2 main.cpp common.h utils.h)

target_link_libraries(u2 ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

