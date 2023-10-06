cmake_minimum_required(VERSION 3.18)

project("GLAD" LANGUAGES C)

add_library(GLAD STATIC
  "src/gl.c"
)

target_include_directories(GLAD PUBLIC "./include")
