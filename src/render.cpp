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
    // positions
    glm::vec3 pos1(-1.0f, 1.0f, 0.0f);
    glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
    glm::vec3 pos3(1.0f, -1.0f, 0.0f);
    glm::vec3 pos4(1.0f, 1.0f, 0.0f);

    // Texture coordinates
    glm::vec2 uv1(0.0f, 1.0f);
    glm::vec2 uv2(0.0f, 0.0f);
    glm::vec2 uv3(1.0f, 0.0f);
    glm::vec2 uv4(1.0f, 1.0f);

    // Normal vector
    glm::vec3 nm(0.0f, 0.0f, 1.0f);

    // Calculate tangent/bitangent vectors of both triangles
    glm::vec3 tangent1{};
    glm::vec3 bitangent1{};
    glm::vec3 tangent2{};
    glm::vec3 bitangent2{};

    // Triangle 1
    glm::vec3 edge1{pos2 - pos1};
    glm::vec3 edge2{pos3 - pos1};
    glm::vec2 deltaUV1{uv2 - uv1};
    glm::vec2 deltaUV2{uv3 - uv1};

    float f{1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y)};

    tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

    bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

    // Triangle 2
    edge1 = pos3 - pos1;
    edge2 = pos4 - pos1;
    deltaUV1 = uv3 - uv1;
    deltaUV2 = uv4 - uv1;

    f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

    bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

    // each row corresponds to a vertex:
    // 3f, 3f, 2f, 3f, 3f -> position, normal, texCoords, tangent, bitangent
    const std::array<float, (3 + 3 + 2 + 3 + 3) * 3 * 2> newQuadVertices{
        pos1.x,       pos1.y,       pos1.z,       nm.x,         nm.y,
        nm.z,         uv1.x,        uv1.y,        tangent1.x,   tangent1.y,
        tangent1.z,   bitangent1.x, bitangent1.y, bitangent1.z, pos2.x,
        pos2.y,       pos2.z,       nm.x,         nm.y,         nm.z,
        uv2.x,        uv2.y,        tangent1.x,   tangent1.y,   tangent1.z,
        bitangent1.x, bitangent1.y, bitangent1.z, pos3.x,       pos3.y,
        pos3.z,       nm.x,         nm.y,         nm.z,         uv3.x,
        uv3.y,        tangent1.x,   tangent1.y,   tangent1.z,   bitangent1.x,
        bitangent1.y, bitangent1.z,

        pos1.x,       pos1.y,       pos1.z,       nm.x,         nm.y,
        nm.z,         uv1.x,        uv1.y,        tangent2.x,   tangent2.y,
        tangent2.z,   bitangent2.x, bitangent2.y, bitangent2.z, pos3.x,
        pos3.y,       pos3.z,       nm.x,         nm.y,         nm.z,
        uv3.x,        uv3.y,        tangent2.x,   tangent2.y,   tangent2.z,
        bitangent2.x, bitangent2.y, bitangent2.z, pos4.x,       pos4.y,
        pos4.z,       nm.x,         nm.y,         nm.z,         uv4.x,
        uv4.y,        tangent2.x,   tangent2.y,   tangent2.z,   bitangent2.x,
        bitangent2.y, bitangent2.z};

    // Configure plane VAO
    glGenVertexArrays(1, &meshes::quadVAO);
    glGenBuffers(1, &meshes::quadVBO);

    // Fill buffer
    glBindBuffer(GL_ARRAY_BUFFER, meshes::quadVBO);
    glBufferData(GL_ARRAY_BUFFER, newQuadVertices.size() * sizeof(float),
                 newQuadVertices.data(), GL_STATIC_DRAW);

    glBindVertexArray(meshes::quadVAO);

    // clang-format off
    constexpr auto quadStride         {14 * sizeof(float)};
    // Offsets
    constexpr auto quadPositionOffset { 0 * sizeof(float)};
    constexpr auto quadNormalOffset   { 3 * sizeof(float)};
    constexpr auto quadTexCoordsOffset{ 6 * sizeof(float)};
    constexpr auto quadTangentOffset  { 8 * sizeof(float)};
    constexpr auto quadBitangentOffset{11 * sizeof(float)};
    // clang-format on

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, quadStride,
                          reinterpret_cast<void*>(quadPositionOffset));

    // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, quadStride,
                          reinterpret_cast<void*>(quadNormalOffset));

    // TexCoords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, quadStride,
                          reinterpret_cast<void*>(quadTexCoordsOffset));

    // Tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, quadStride,
                          reinterpret_cast<void*>(quadTangentOffset));

    // Bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, quadStride,
                          reinterpret_cast<void*>(quadBitangentOffset));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  // Render quad
  glBindVertexArray(meshes::quadVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
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
