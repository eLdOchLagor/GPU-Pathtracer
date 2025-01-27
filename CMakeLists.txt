cmake_minimum_required(VERSION 3.15)

project(Raytracer)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories for binaries and libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(include)

# Automatically gather all source files under src/
file(GLOB_RECURSE SRC_FILES src/*.cpp)

# Create the executable
add_executable(Raytracer ${SRC_FILES})

# Include GLFW and GLAD
add_subdirectory(external/glfw)
add_library(GLAD external/glad/src/glad.c)
target_include_directories(GLAD PUBLIC external/glad/include)

# Link libraries to the project
target_link_libraries(Raytracer PRIVATE glfw GLAD)

# Add OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(Raytracer PRIVATE OpenGL::GL)