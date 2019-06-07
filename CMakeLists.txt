cmake_minimum_required(VERSION 3.13)
set(PROJECT_NAME HJRT)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 14)

#set include directories
include_directories(include)
include_directories(3rd-party)
#set source file idrectories
aux_source_directory(${PROJECT_SOURCE_DIR}/src HJRT_SRCS)
#set output
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
add_executable(${PROJECT_NAME} ${HJRT_SRCS} )
