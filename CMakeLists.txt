cmake_minimum_required(VERSION 3.10)
project(bouncer)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)

# add the executable
add_executable(
    bouncer 
    main.cpp 
    glad.c
)

# GLFW
set(GLFW_DIR "${PROJECT_SOURCE_DIR}/libraries")
set(GLFW_INCLUDE_DIR "${GLFW_DIR}/include")
set(GLFW_LIBRARY_DIR "${GLFW_DIR}/lib")
set(GLFW_LIBRARY "${GLFW_LIBRARY_DIR}/libglfw3dll.a")

# GLAD
set(GLAD_INCLUDE_DIR "${PROJECT_SOURCE_DIR}")

# GLM
set(GLM_INCLUDE_DIR "${GLFW_DIR}/include")

# include directories
include_directories(${GLFW_INCLUDE_DIR} ${GLAD_INCLUDE_DIR} ${GLM_INCLUDE_DIR})

# link libraries
target_link_libraries(bouncer ${GLFW_LIBRARY})
