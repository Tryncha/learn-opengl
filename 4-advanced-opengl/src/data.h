#ifndef DATA_H
#define DATA_H

#include <array>
#include <glm/glm.hpp>

// clang-format off
namespace data {
// vertex data
// each row corresponds to a vertex:
// 3 floats, 2 floats -> position, texture coords
constexpr std::array<float, 5 * 6 * 6> cubeVertices{
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
   0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
  -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

// each row corresponds to a vertex:
// 3 floats, 2 floats -> position, texture coords.
// Note we set these higher than 1
// (together with GL_REPEAT as texture wrapping mode).
// This will cause the floor texture to repeat.
constexpr std::array<float, 5 * 3 * 2> planeVertices{
   5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
  -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
  -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

   5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
  -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
   5.0f, -0.5f, -5.0f,  2.0f, 2.0f
};

// Each row corresponds to a vertex:
// 3 floats, 2 floats -> position, texture coords,
// (swapped texture y-coordinates because texture is flipped upside down)
constexpr std::array<float, 5 * 3 * 2> transparentVertices{
    0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
    0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
    1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

    0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
    1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
    1.0f,  0.5f,  0.0f,  1.0f,  0.0f
};

constexpr std::array<glm::vec3, 5> grassPositions{
  glm::vec3(-1.5f, 0.0f, -0.48f),
  glm::vec3( 1.5f, 0.0f,  0.51f),
  glm::vec3( 0.0f, 0.0f,  0.7f),
  glm::vec3(-0.3f, 0.0f, -2.3f),
  glm::vec3( 0.5f, 0.0f, -0.6f)
};
} // namespace data
// clang-format on

#endif
