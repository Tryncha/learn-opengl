#include "render.h"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "data.h"
#include "shader.h"

// clang-format off
namespace cube {
constexpr auto stride   {8 * sizeof(float)};
namespace offsets{
constexpr auto position {0};
constexpr auto normal   {3 * sizeof(float)};
constexpr auto texCoords{6 * sizeof(float)};
} // namespace offsets
} // namespace cube
// clang-format on

// Renders a 1x1 3D cube in NDC
void renderCube() {
  if (!meshes::cubeVAO) {
    glGenVertexArrays(1, &meshes::cubeVAO);
    glGenBuffers(1, &meshes::cubeVBO);

    // Fill buffer
    glBindBuffer(GL_ARRAY_BUFFER, meshes::cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, data::cubeVertices.size() * sizeof(float),
                 data::cubeVertices.data(), GL_STATIC_DRAW);

    // Link vertex attributes
    glBindVertexArray(meshes::cubeVAO);

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, cube::stride,
                          reinterpret_cast<void*>(cube::offsets::position));

    // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, cube::stride,
                          reinterpret_cast<void*>(cube::offsets::normal));

    // TexCoords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, cube::stride,
                          reinterpret_cast<void*>(cube::offsets::texCoords));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  // Render cube
  glBindVertexArray(meshes::cubeVAO);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
}

// clang-format off
namespace quad {
constexpr auto stride   {5 * sizeof(float)};
namespace offsets{
constexpr auto position {0};
constexpr auto texCoords{3 * sizeof(float)};
} // namespace offsets
} // namespace quad
// clang-format on

// Renders a 1x1 XY quad in NDC
void renderQuad() {
  if (!meshes::quadVAO) {
    glGenVertexArrays(1, &meshes::quadVAO);
    glGenBuffers(1, &meshes::quadVBO);

    // Fill buffer
    glBindBuffer(GL_ARRAY_BUFFER, meshes::quadVBO);
    glBufferData(GL_ARRAY_BUFFER, data::quadVertices.size() * sizeof(float),
                 data::quadVertices.data(), GL_STATIC_DRAW);

    glBindVertexArray(meshes::quadVAO);

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, quad::stride,
                          reinterpret_cast<void*>(quad::offsets::position));

    // TexCoords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, quad::stride,
                          reinterpret_cast<void*>(quad::offsets::texCoords));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  // Render quad
  glBindVertexArray(meshes::quadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

// Renders the 3D scene
void renderScene(const Shader& shader) {
  glm::mat4 model{glm::mat4(1.0f)};

  // Floor
  shader.setMat4("u_Model", model);
  glBindVertexArray(meshes::planeVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  // Cubes
  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0f));
  model = glm::scale(model, glm::vec3(0.5f));
  shader.setMat4("u_Model", model);
  renderCube();

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0f));
  model = glm::scale(model, glm::vec3(0.5f));
  shader.setMat4("u_Model", model);
  renderCube();

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0f));
  model = glm::rotate(model, glm::radians(60.0f),
                      glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));
  model = glm::scale(model, glm::vec3(0.25f));
  shader.setMat4("u_Model", model);
  renderCube();
}
