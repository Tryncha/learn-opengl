#include "mesh.h"

#include <glad/glad.h>

#include <cstddef>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "shader.h"

Mesh::Mesh(const std::vector<Vertex>& vertices,
           const std::vector<Texture>& textures,
           const std::vector<unsigned int>& indices)
    : m_vertices{vertices}, m_textures{textures}, m_indices{indices} {
  setupMesh();
};

void Mesh::setupMesh() {
  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);
  glGenBuffers(1, &m_ebo);

  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER,
               static_cast<std::ptrdiff_t>(m_vertices.size() * sizeof(Vertex)),
               m_vertices.data(), GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0);

  // normal vector attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void*>(offsetof(Vertex, normal)));
  glEnableVertexAttribArray(1);

  // texture coords attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void*>(offsetof(Vertex, texCoords)));
  glEnableVertexAttribArray(2);

  glBindVertexArray(0);
}

void Mesh::draw(const Shader& shader) {
  int diffuseNr{1};
  int specularNr{1};

  for (std::size_t i{0}; i < m_textures.size(); ++i) {
    // activate proper texture unit before binding
    glActiveTexture(GL_TEXTURE0 + static_cast<unsigned int>(i));

    // retrieve texture number (the N in diffuse_textureN)
    std::string number{};
    std::string name{m_textures[i].type};

    if (name == "texture_diffuse") {
      number = std::to_string(diffuseNr);
      diffuseNr++;
    } else if (name == "texture_specular") {
      number = std::to_string(specularNr);
      specularNr++;
    }

    shader.setInt(("material." + name + number).c_str(), static_cast<int>(i));
    glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
  }

  glActiveTexture(GL_TEXTURE0);

  glBindVertexArray(m_vao);
  glDrawElements(GL_TRIANGLES, static_cast<int>(m_indices.size()),
                 GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}
