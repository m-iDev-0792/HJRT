cmake_minimum_required(VERSION 3.12)
set(PROJECT_NAME HJRT)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Release)
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
#GLFW3.3
link_directories(3rd-party/GLFW/lib)
IF(WIN32)
    link_libraries(glfw3.lib)
ELSEIF(APPLE)
    set(GLFW3_LIB)
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    find_library(IOKIT_LIBRARY IOKit REQUIRED)
    find_library(COREVID_LIBRARY CoreVideo REQUIRED)
    link_libraries(libglfw3.a)
ENDIF()

#-----------set output-----------
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
add_executable(${PROJECT_NAME} ${HJRT_SRCS} ${GLAD_SRCS} ${IMGUI_SRCS})
IF(APPLE)
target_link_libraries(${PROJECT_NAME} ${COCOA_LIBRARY} ${COREVID_LIBRARY} ${IOKIT_LIBRARY})
ENDIF()
