#ifndef MATERIALS_H
#define MATERIALS_H

#include <glm/glm.hpp>

struct Material {
  glm::vec3 ambient;
  glm::vec3 diffuse;
  glm::vec3 specular;
  float shininess;
};

// clang-format off
namespace materials {
constexpr Material cyanPlastic{
  glm::vec3(0.0f, 0.1f, 0.06f),
  glm::vec3(0.0f, 0.50980392f, 0.50980392f),
  glm::vec3(0.50196078f, 0.50196078f, 0.50196078f),
  0.25f
};

constexpr Material emerald{
  glm::vec3(0.0215f, 0.1745f, 0.0215f),
  glm::vec3(0.07568f, 0.61424f, 0.07568f),
  glm::vec3(0.633f, 0.727811f, 0.633f),
  0.6f
};

constexpr Material obsidian{
  glm::vec3(0.05375f, 0.05f, 0.06625f),
  glm::vec3(0.18275f, 0.17f, 0.22525f),
  glm::vec3(0.332741f, 0.328634f, 0.346435f),
  0.3f
};

constexpr Material pearl{
  glm::vec3(0.25f, 0.20725f, 0.20725f),
  glm::vec3(1.0f, 0.829f, 0.829f),
  glm::vec3(0.296648f, 0.296648f, 0.296648f),
  0.088f
};

constexpr Material gold{
  glm::vec3(0.24725f, 0.1995f, 0.0745f),
  glm::vec3(0.75164f, 0.60648f, 0.22648f),
  glm::vec3(0.628281f, 0.555802f, 0.366065f),
  0.4f
};
}  // namespace materials
// clang-format on

#endif
