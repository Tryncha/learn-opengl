#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "shader.h"

struct Vertex {
  glm::vec3 position{};
  glm::vec3 normal{};
  glm::vec3 texCoords{};
};

struct Texture {
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
  unsigned int m_vao{};
  unsigned int m_vbo{};
  unsigned int m_ebo{};

  std::vector<Vertex> m_vertices;
  std::vector<Texture> m_textures;
  std::vector<unsigned int> m_indices;

  void setupMesh();
};
