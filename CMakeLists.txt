cmake_minimum_required(VERSION 3.18)

if (NOT WIN32)
  message(FATAL_ERROR
    "This project is windows only"
  )
endif()

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory("glfw")
add_subdirectory("glad")

find_package(OpenGL REQUIRED)

project(GraviSim LANGUAGES CXX)

add_executable(gravisim
  "main.cpp"

  "Planet.cpp"
  "Planet.h"

  "Rendering.cpp"
  "Rendering.h"

  "Shaders.cpp"
  "Shaders.h"

  "Ticker.cpp"
  "Ticker.h"

  "Vector.h"
)

target_link_libraries(gravisim PRIVATE glfw OpenGL::GL GLAD)
