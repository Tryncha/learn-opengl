#ifndef MESH_H
#define MESH_H

#include <array>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "shader.h"

struct Vertex {
  static constexpr int s_max_bone_influence{4};

  glm::vec3 position{};
  glm::vec3 normal{};
  glm::vec2 texCoords{};
  glm::vec3 tangent{};
  glm::vec3 bitangent{};
  std::array<int, s_max_bone_influence> boneIds{};
  std::array<float, s_max_bone_influence> weights{};
};

struct Texture {
  std::string path{};
  std::string type{};
  unsigned int id{};
};

class Mesh {
 public:
  Mesh(const std::vector<Vertex>& vertices,
       const std::vector<Texture>& textures,
       const std::vector<unsigned int>& indices);

  void draw(const Shader& shader);

 private:
  void setupMesh();

  std::vector<Vertex> m_vertices;
  std::vector<Texture> m_textures;
  std::vector<unsigned int> m_indices;

  unsigned int m_vao{};
  unsigned int m_vbo{};
  unsigned int m_ebo{};
};

#endif
