cmake_minimum_required(VERSION 3.13)
set(PROJECT_NAME HJRT)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 14)

#-----------set include directories-----------
include_directories(include)
include_directories(3rd-party)

#-----------set source file idrectories-----------
aux_source_directory(${PROJECT_SOURCE_DIR}/src HJRT_SRCS)

#-----------set up 3rd-party lib src-----------
#imgui src
aux_source_directory(${PROJECT_SOURCE_DIR}/3rd-party/imgui/src IMGUI_SRCS)
#glad src
aux_source_directory(${PROJECT_SOURCE_DIR}/3rd-party/glad/src GLAD_SRCS)

#-----------set up 3rd-party lib link-----------
#GLFW3.2
link_directories(3rd-party/GLFW/lib)
set(GLFW3_LIB libglfw.3.2.dylib)

#-----------set output-----------
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
add_executable(${PROJECT_NAME} ${HJRT_SRCS} ${GLAD_SRCS} ${IMGUI_SRCS})
target_link_libraries(${PROJECT_NAME} ${GLFW3_LIB})
