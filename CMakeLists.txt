cmake_minimum_required(VERSION 3.10)
project(DotsAndBoxes)

set(CMAKE_CXX_STANDARD 11)

add_definitions(-DFREEGLUT_STATIC)

include_directories(include third_party/freeglut/include third_party/glui/include)

link_directories(third_party/freeglut/lib third_party/glui/lib)

add_executable(DotsAndBoxes main.cpp)

set_target_properties(DotsAndBoxes PROPERTIES LINK_FLAGS "-static" )

target_link_libraries(DotsAndBoxes glui freeglut_static opengl32 glu32 winmm gdi32)