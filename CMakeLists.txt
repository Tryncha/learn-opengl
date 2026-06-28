cmake_minimum_required(VERSION 3.10.0)
project(learn-opengl VERSION 0.1.0 LANGUAGES C CXX)

find_package(glfw3 REQUIRED)

add_executable(
  ${PROJECT_NAME}
  src/glad.c
  src/main.cpp
  src/shader.cpp
  src/stb_image.cpp
)

target_include_directories(
  ${PROJECT_NAME} PRIVATE include
)

target_compile_features(
  ${PROJECT_NAME} PRIVATE cxx_std_23
)

target_link_libraries(
  ${PROJECT_NAME} PRIVATE
  glfw
  opengl32
)

target_compile_options(
  ${PROJECT_NAME} PRIVATE
  -Wall
  -Wextra
  -Wpedantic
  -Wconversion
  -Wsign-conversion
  -Wshadow
)

target_compile_options(
  ${PROJECT_NAME} PRIVATE
  $<$<CONFIG:Debug>:-g>
  $<$<CONFIG:Debug>:-ggdb>
  $<$<CONFIG:Release>:-O2>
  $<$<CONFIG:Release>:-DNDEBUG>
)

# Ignore warnings from glad.c
set_source_files_properties(
  src/glad.c
  PROPERTIES
  COMPILE_OPTIONS "-w"
)
