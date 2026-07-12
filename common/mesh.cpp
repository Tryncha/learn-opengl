#include "mesh.h"

#include <glad/glad.h>

#include <cstddef>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "shader.h"

Mesh::Mesh(const std::vector<Vertex>& vertices,
           const std::vector<unsigned int>& indices,
           const std::vector<Texture>& textures)
    : m_vertices{vertices}, m_indices{indices}, m_textures{textures} {
  setupMesh();
};

void Mesh::draw(const Shader& shader) {
  int diffuseNr{1};
  int specularNr{1};
  int normalNr{1};
  int heightNr{1};

  for (std::size_t i{0}; i < m_textures.size(); ++i) {
    // activate proper texture unit before binding
    glActiveTexture(GL_TEXTURE0 + static_cast<unsigned int>(i));

    // retrieve texture number (the N in diffuse_textureN)
    std::string number{};
    std::string name{m_textures[i].type};

    if (name == "texture_diffuse") {
      // transfer int to string
      number = std::to_string(diffuseNr);
      diffuseNr++;
    } else if (name == "texture_specular") {
      number = std::to_string(specularNr);
      specularNr++;
    } else if (name == "texture_normal") {
      number = std::to_string(normalNr);
      normalNr++;
    } else if (name == "texture_height") {
      number = std::to_string(heightNr);
      heightNr++;
    }

    // now set the sampler to the correct texture unit
    shader.setInt(("material." + name + number).c_str(), static_cast<int>(i));
    // and finally bind the texture
    glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
  }

  // draw mesh
  glBindVertexArray(m_vao);
  glDrawElements(GL_TRIANGLES, static_cast<int>(m_indices.size()),
                 GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);

  // always good practice to set everything back to defaults once configured.
  glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh() {
  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);
  glGenBuffers(1, &m_ebo);

  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER,
               static_cast<std::ptrdiff_t>(m_vertices.size() * sizeof(Vertex)),
               m_vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  glBufferData(
      GL_ELEMENT_ARRAY_BUFFER,
      static_cast<std::ptrdiff_t>(m_indices.size() * sizeof(unsigned int)),
      m_indices.data(), GL_STATIC_DRAW);

  // position attribute
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void*>(0));

  // normal vector attribute
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void*>(offsetof(Vertex, normal)));

  // texture coords attribute
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void*>(offsetof(Vertex, texCoords)));

  // tangent attribute
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void*>(offsetof(Vertex, tangent)));

  // bitangent attribute
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void*>(offsetof(Vertex, bitangent)));

  // boneIds attribute
  glEnableVertexAttribArray(5);
  glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex),
                         reinterpret_cast<void*>(offsetof(Vertex, boneIds)));

  // weights attribute
  glEnableVertexAttribArray(6);
  glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void*>(offsetof(Vertex, weights)));

  glBindVertexArray(0);
}
