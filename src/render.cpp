#include "render.h"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "data.h"
#include "shader.h"

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

    // clang-format off
    constexpr auto cubeStride         {8 * sizeof(float)};
    // Offsets
    constexpr auto cubePositionOffset {0 * sizeof(float)};
    constexpr auto cubeNormalOffset   {3 * sizeof(float)};
    constexpr auto cubeTexCoordsOffset{6 * sizeof(float)};
    // clang-format on

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, cubeStride,
                          reinterpret_cast<void*>(cubePositionOffset));

    // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, cubeStride,
                          reinterpret_cast<void*>(cubeNormalOffset));

    // TexCoords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, cubeStride,
                          reinterpret_cast<void*>(cubeTexCoordsOffset));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  // Render cube
  glBindVertexArray(meshes::cubeVAO);
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
}

// Renders a 1x1 XY quad in NDC
void renderQuad() {
  if (!meshes::quadVAO) {
    // Configure quad VAO
    glGenVertexArrays(1, &meshes::quadVAO);
    glGenBuffers(1, &meshes::quadVBO);

    // Fill buffer
    glBindBuffer(GL_ARRAY_BUFFER, meshes::quadVBO);
    glBufferData(GL_ARRAY_BUFFER, data::quadVertices.size() * sizeof(float),
                 data::quadVertices.data(), GL_STATIC_DRAW);

    glBindVertexArray(meshes::quadVAO);

    // clang-format off
    constexpr auto quadStride         {5 * sizeof(float)};
    // Offsets
    constexpr auto quadPositionOffset {0 * sizeof(float)};
    constexpr auto quadTexCoordsOffset{3 * sizeof(float)};
    // clang-format on

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, quadStride,
                          reinterpret_cast<void*>(quadPositionOffset));

    // TexCoords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, quadStride,
                          reinterpret_cast<void*>(quadTexCoordsOffset));

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

  // Room cube
  model = glm::mat4(1.0f);
  model = glm::scale(model, glm::vec3(5.0f));

  shader.setMat4("u_Model", model);

  // Note that we disable culling here since we render
  // 'inside' the cube instead of the usual 'outside'
  // which throws off the normal culling methods.
  glDisable(GL_CULL_FACE);

  // A small little hack to invert normals when drawing cube
  // from the inside so lighting still works.
  shader.setBool("u_ReverseNormals", true);
  renderCube();
  // and of course disable it
  shader.setBool("u_ReverseNormals", false);

  glEnable(GL_CULL_FACE);

  // Cubes
  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(4.0f, -3.5f, 0.0f));
  model = glm::scale(model, glm::vec3(0.5f));
  shader.setMat4("u_Model", model);
  renderCube();

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(2.0f, 3.0f, 1.0f));
  model = glm::scale(model, glm::vec3(0.75f));
  shader.setMat4("u_Model", model);
  renderCube();

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(-3.0f, -1.0f, 0.0f));
  model = glm::scale(model, glm::vec3(0.5f));
  shader.setMat4("u_Model", model);
  renderCube();

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(-1.5f, 1.0f, 1.5f));
  model = glm::scale(model, glm::vec3(0.5f));
  shader.setMat4("u_Model", model);
  renderCube();

  model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(-1.5f, 2.0f, -3.0));
  model = glm::rotate(model, glm::radians(60.0f),
                      glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));
  model = glm::scale(model, glm::vec3(0.75f));
  shader.setMat4("u_Model", model);
  renderCube();
}
