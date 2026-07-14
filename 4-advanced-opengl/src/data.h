#ifndef DATA_H
#define DATA_H

#include <array>
#include <glm/glm.hpp>

// clang-format off
namespace data {
// vertex data

/*
Remember: to specify vertices in a counter-clockwise winding order you need
to visualize the triangle as if you're in front of the triangle and from that
point of view, is where you set their order.

To define the order of a triangle on the right side of the cube for example,
you'd imagine yourself looking straight at the right side of the cube, and
then visualize the triangle and make sure their order is specified in a
counter-clockwise order. This takes some practice, but try visualizing this
yourself and see that this is correct.
*/

// each row corresponds to a vertex:
// 3 floats, 2 floats -> position, texture coords
constexpr std::array<float, 5 * 6 * 6> cubeVertices{
  // Back face
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // bottom-left
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // top-right
   0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  // bottom-right         
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // top-right
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // bottom-left
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  // top-left
  // Front face
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // bottom-left
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  // bottom-right
   0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  // top-right
   0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  // top-right
  -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  // top-left
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // bottom-left
  // Left face
  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  // top-right
  -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // top-left
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  // bottom-left
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  // bottom-left
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // bottom-right
  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  // top-right
  // Right face
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  // top-left
   0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  // bottom-right
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // top-right         
   0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  // bottom-right
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  // top-left
   0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // bottom-left     
  // Bottom face
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  // top-right
   0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  // top-left
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  // bottom-left
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  // bottom-left
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // bottom-right
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  // top-right
  // Top face
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  // top-left
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  // bottom-right
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // top-right     
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  // bottom-right
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  // top-left
  -0.5f,  0.5f,  0.5f,  0.0f, 0.0f   // bottom-left 
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

constexpr std::array<glm::vec3, 5> windowPositions{
  glm::vec3(-1.5f, 0.0f, -0.48f),
  glm::vec3( 1.5f, 0.0f,  0.51f),
  glm::vec3( 0.0f, 0.0f,  0.7f),
  glm::vec3(-0.3f, 0.0f, -2.3f),
  glm::vec3( 0.5f, 0.0f, -0.6f)
};
} // namespace data
// clang-format on

#endif
