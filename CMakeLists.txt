cmake_minimum_required(VERSION 3.12.0)
project(learn-opengl VERSION 0.1.0 LANGUAGES C CXX)

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(
  lgl-common STATIC
  common/glad.c
  common/stb_image.cpp
  common/camera.cpp
  common/mesh.cpp
  common/model.cpp
  common/shader.cpp
)

target_include_directories(
  lgl-common PUBLIC
  include
  common
)

target_link_libraries(
  lgl-common PUBLIC
  glfw
  opengl32
  glm::glm
  assimp::assimp
)

# Ignore warnings from glad.c
set_source_files_properties(
  common/glad.c
  PROPERTIES
  COMPILE_OPTIONS "-w"
)

function(add_chapter_target chapter_name chapter_sources)
  add_executable(
    ${chapter_name}
    ${chapter_sources}
  )

  target_link_libraries(
    ${chapter_name} PRIVATE
    lgl-common
  )

  target_compile_options(
    ${chapter_name} PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wsign-conversion
    -Wshadow
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Debug>:-ggdb>
    $<$<CONFIG:Release>:-O2>
    $<$<CONFIG:Release>:-DNDEBUG>
  )

  target_compile_definitions(
    ${chapter_name} PRIVATE
    CHAPTER_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
  )

  set_target_properties(
    ${chapter_name} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
  )
endfunction()

add_subdirectory(1-getting-started)
add_subdirectory(2-lighting)
add_subdirectory(3-model-loading)
add_subdirectory(4-advanced-opengl)
