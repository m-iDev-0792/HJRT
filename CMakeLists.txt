cmake_minimum_required(VERSION 3.13)
project(RTTest)
include_directories(/usr/local/include)
set(CMAKE_CXX_STANDARD 14)

add_executable(RTTest main.cpp camera.cpp objects.cpp scene.cpp material.cpp utility.cpp imageExporter.cpp aabb.cpp bvh.cpp bvh.h)