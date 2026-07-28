#ifndef DATA_H
#define DATA_H

#include <array>

// clang-format off
namespace data {
// each row corresponds to a vertex:
// 3f, 3f, 2f -> position, normal, texCoords
constexpr std::array<float, (3 + 3 + 2) * 3 * 2> planeVertices{
   25.0f, -0.5f,  25.0f, 0.0f, 1.0f, 0.0f, 25.0f,  0.0f,
  -25.0f, -0.5f,  25.0f, 0.0f, 1.0f, 0.0f,  0.0f,  0.0f,
  -25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f,  0.0f, 25.0f,

   25.0f, -0.5f,  25.0f,  .0f, 1.0f, 0.0f, 25.0f,  0.0f,
  -25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f,  0.0f, 25.0f,
   25.0f, -0.5f, -25.0f, 0.0f, 1.0f, 0.0f, 25.0f, 25.0f
};

// each row corresponds to a vertex:
// 3f, 3f, 2f -> position, normal, texCoords
constexpr std::array<float, (3 + 3 + 2) * 6 * 6> cubeVertices{
  // back face
  -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
   1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,  // top-right
   1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,  // bottom-right         
   1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,  // top-right
  -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
  -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,  // top-left
  // front face
  -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,  // bottom-left
   1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,  // bottom-right
   1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,  // top-right
   1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,  // top-right
  -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,  // top-left
  -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,  // bottom-left
  // left face
  -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,  // top-right
  -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,  // top-left
  -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,  // bottom-left
  -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,  // bottom-left
  -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,  // bottom-right
  -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,  // top-right
  // right face
   1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,  // top-left
   1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,  // bottom-right
   1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,  // top-right         
   1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,  // bottom-right
   1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,  // top-left
   1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,  // bottom-left     
  // bottom face
  -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,  // top-right
   1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,  // top-left
   1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,  // bottom-left
   1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,  // bottom-left
  -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,  // bottom-right
  -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,  // top-right
  // top face
  -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,  // top-left
   1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,  // bottom-right
   1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,  // top-right     
   1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,  // bottom-right
  -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,  // top-left
  -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f   // bottom-left        
};

// each row corresponds to a vertex:
// 3f, 2f -> position, texCoords
constexpr std::array<float, (3 + 2) * 4> quadVertices{
  -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
  -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
   1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
   1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
};
} // namespace data
// clang-format on

#endif
