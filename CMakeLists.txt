cmake_minimum_required(VERSION 3.13)
project(RTTest)
include_directories(/usr/local/include)
set(CMAKE_CXX_STANDARD 14)

add_executable(RTTest main.cpp Camera.cpp Objects.cpp Scene.cpp Material.cpp Material.h utility.cpp)