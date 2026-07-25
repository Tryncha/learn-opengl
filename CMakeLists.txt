cmake_minimum_required(VERSION 3.20)
project(learn-opengl VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# glfw
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.4
)

# glm
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 1.0.3
)

# assimp
FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG v6.0.2
)

# Reduce assimp compile time
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(
  glfw
  glm
  assimp
)

add_executable(${PROJECT_NAME}
  src/main.cpp
  src/camera.cpp
  src/callbacks.cpp
  src/mesh.cpp
  src/model.cpp
  src/shader.cpp
  src/stb_image.cpp
  src/textures.cpp
)

add_library(glad STATIC
  third_party/glad/src/glad.c
)

target_include_directories(glad SYSTEM PUBLIC
  third_party/glad/include
)

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
  third_party/stb_image
)

target_link_libraries(${PROJECT_NAME} PRIVATE
  glfw
  glad
  opengl32
  glm::glm
  assimp
)

# Compile options
target_compile_options(${PROJECT_NAME} PRIVATE
  -Wall
  -Wextra
  -Wpedantic
  -Wconversion
  -Wshadow
)

target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<CONFIG:Debug>:-g>
  $<$<CONFIG:Debug>:-ggdb>
  $<$<CONFIG:Release>:-O2>
  $<$<CONFIG:Release>:-DNDEBUG>
)
