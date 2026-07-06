#ifndef DATA_H
#define DATA_H

#include <array>
#include <glm/glm.hpp>

// clang-format off
namespace data {
// vertex data
// each row corresponds to a vertex:
// 3 floats, 3 floats, 2 floats
// -> position, normal vector on that vertex, texture coords
constexpr std::array<float, 8 * 6 * 6> vertexData{
  -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
   0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
   0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
  -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,

  -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
   0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
   0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
  -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,

  -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
  -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
  -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
  -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
  -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

   0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
   0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

  -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
   0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
   0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,

  -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
   0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
   0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
  -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
  -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f
};

constexpr std::array<glm::vec3, 10> cubePositions{{
  { 0.0f,  0.0f,   0.0f},
  { 2.0f,  5.0f, -15.0f},
  {-1.5f, -2.2f,  -2.5f},
  {-3.8f, -2.0f, -12.3f},
  { 2.4f, -0.4f,  -3.5f},
  {-1.7f,  3.0f,  -7.5f},
  { 1.3f, -2.0f,  -2.5f},
  { 1.5f,  2.0f,  -2.5f},
  { 1.5f,  0.2f,  -1.5f},
  {-1.3f,  1.0f,  -1.5f}
}};

// lighting data
struct DirLight {
  glm::vec3 dir;
  // lights
  glm::vec3 ambt;
  glm::vec3 diff;
  glm::vec3 spec;
};

struct PointLight {
  glm::vec3 pos;
  // lights
  glm::vec3 ambt;
  glm::vec3 diff;
  glm::vec3 spec;
  // attenuation values
  float constant;
  float linear;
  float quadratic;
};

struct SpotLight {
  glm::vec3 pos;
  glm::vec3 dir;
  // lights
  glm::vec3 ambt;
  glm::vec3 diff;
  glm::vec3 spec;
  // attenuation values
  float constant;
  float linear;
  float quadratic;
  // spotlight cutoff
  float innerCutOff;
  float outerCutOff;
};

const DirLight dirLight{
  // position
  glm::vec3{-0.2f, -1.0f, -0.3f},
  // lights
  glm::vec3{0.05f, 0.05f, 0.05f},
  glm::vec3{0.4f,  0.4f,  0.4f},
  glm::vec3{0.5f,  0.5f,  0.5f}
};

const std::array<PointLight, 4> pointLights{
  // point light 1
  PointLight{
    // position
    glm::vec3{0.7f, 0.2f, 2.0f},
    // lights
    glm::vec3{0.05f, 0.05f, 0.05f},
    glm::vec3{0.8f,  0.8f,  0.8f}, 
    glm::vec3{1.0f,  1.0f,  1.0f},
    // attenuation values
    1.0f,
    0.09f,
    0.032f
  },
  // point light 2
  PointLight{
    // position
    glm::vec3{2.3f, -3.3f, -4.0f},
    // lights
    glm::vec3{0.05f, 0.05f, 0.05f},
    glm::vec3{0.8f,  0.8f,  0.8f}, 
    glm::vec3{1.0f,  1.0f,  1.0f},
    // attenuation values
    1.0f,
    0.09f,
    0.032f
  },
  // point light 3
  PointLight{
    // position
    glm::vec3{-4.0f, 2.0f, -12.0f},
    // lights
    glm::vec3{0.05f, 0.05f, 0.05f},
    glm::vec3{0.8f,  0.8f,  0.8f}, 
    glm::vec3{1.0f,  1.0f,  1.0f},
    // attenuation values
    1.0f,
    0.09f,
    0.032f
  },
  // point light 4
  PointLight{
    // position
    glm::vec3{ 0.0f, 0.0f, -3.0f},
    // lights
    glm::vec3{0.05f, 0.05f, 0.05f},
    glm::vec3{0.8f,  0.8f,  0.8f}, 
    glm::vec3{1.0f,  1.0f,  1.0f},
    // attenuation values
    1.0f,
    0.09f,
    0.032f
  }
};

const SpotLight spotLight{
  // position and direction
  glm::vec3{0.0f, 0.0f, 0.0f},
  glm::vec3{1.0f, 1.0f, 1.0f},
  // lights
  glm::vec3{0.0f, 0.0f, 0.0f},
  glm::vec3{1.0f, 1.0f, 1.0f},
  glm::vec3{1.0f, 1.0f, 1.0f},
  // attenuation values
  1.0f,
  0.09f,
  0.032f,
  // spotlight cutoff
  std::cos(glm::radians(12.5f)),
  std::cos(glm::radians(17.5f))
};
} // namespace data
// clang-format on

#endif
