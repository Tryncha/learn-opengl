#ifndef DATA_H
#define DATA_H

#include <array>

// clang-format off
namespace data {
// vertex data
// each row corresponds to a vertex:
// 3f, 3f, 2f -> position, normal, texCoords
constexpr std::array<float, (3 + 3 + 2) * 3 * 2> planeVertices{
   10.0f, -0.5f,  10.0f, 0.0f, 1.0f, 0.0f, 10.0f,  0.0f,
  -10.0f, -0.5f,  10.0f, 0.0f, 1.0f, 0.0f,  0.0f,  0.0f,
  -10.0f, -0.5f, -10.0f, 0.0f, 1.0f, 0.0f,  0.0f, 10.0f,

   10.0f, -0.5f,  10.0f, 0.0f, 1.0f, 0.0f, 10.0f,  0.0f,
  -10.0f, -0.5f, -10.0f, 0.0f, 1.0f, 0.0f,  0.0f, 10.0f,
   10.0f, -0.5f, -10.0f, 0.0f, 1.0f, 0.0f, 10.0f, 10.0f
};
} // namespace data
// clang-format on

#endif
