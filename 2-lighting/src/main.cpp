// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <stb_image/stb_image.h>

#include <array>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>

#include "camera.h"
#include "data.h"
#include "shader.h"

// clang-format off
namespace window {
// fullscreen may cause bugs, disabled by default
constexpr bool openFullscreen{false};
constexpr int width {1920};
constexpr int height{1080};
constexpr float aspectRatio{static_cast<float>(width) /
                            static_cast<float>(height)};
}  // namespace window

namespace cursor {
inline bool isFirstInput{true};
inline float lastX{window::width  / 2};
inline float lastY{window::height / 2};
}  // namespace cursor
// clang-format on

// GLFW callbacks
void framebufferSizeCallback([[maybe_unused]] GLFWwindow* window, int width,
                             int height) {
  glViewport(0, 0, width, height);
}

// mouse/cursor controls
void cursorPosCallback([[maybe_unused]] GLFWwindow* window, double posX,
                       double posY) {
  auto& camera{*static_cast<Camera*>(glfwGetWindowUserPointer(window))};

  if (cursor::isFirstInput) {
    cursor::lastX = static_cast<float>(posX);
    cursor::lastY = static_cast<float>(posY);
    cursor::isFirstInput = false;
  }

  float offsetX{static_cast<float>(posX - cursor::lastX)};
  // y-coords range from bottom to top
  float offsetY{static_cast<float>(cursor::lastY - posY)};

  cursor::lastX = static_cast<float>(posX);
  cursor::lastY = static_cast<float>(posY);

  camera.processMouseInput(offsetX, offsetY);
}

// offsetY means how much we scrolled vertically
void scrollCallback([[maybe_unused]] GLFWwindow* window,
                    [[maybe_unused]] double offsetX, double offsetY) {
  auto& camera{*static_cast<Camera*>(glfwGetWindowUserPointer(window))};
  camera.processScrollInput(offsetY);
}

// process input
void processInput(GLFWwindow* window) {
  auto& camera{*static_cast<Camera*>(glfwGetWindowUserPointer(window))};

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  camera.processKeyboardInput(window);
}

// utility functions
unsigned int loadTexture(const char* texturePath, bool flipVertically = false) {
  int width{};
  int height{};
  int nrChannels{};

  unsigned int textureId{};
  glGenTextures(1, &textureId);

  unsigned char* textureData{
      stbi_load(texturePath, &width, &height, &nrChannels, 0)};

  if (textureData) {
    unsigned int format{};
    switch (nrChannels) {
      case 1:
        format = GL_RED;
        break;
      case 3:
        format = GL_RGB;
        break;
      case 4:
        format = GL_RGBA;
        break;
    }

    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<int>(format), width, height, 0,
                 format, GL_UNSIGNED_BYTE, textureData);

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(flipVertically);
  } else {
    std::cerr << "Failed to load texture.\nCheck path: " << texturePath << '\n';
  }

  stbi_image_free(textureData);
  return textureId;
}

int main(int, char**) {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW.\n";
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

  GLFWwindow* window{glfwCreateWindow(
      window::width, window::height, "LearnOpenGL",
      window::openFullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr)};

  if (!window) {
    std::cout << "Failed to create GLFW window.\n";
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  Camera camera{glm::vec3(0.0f, 0.0f, 3.0f)};

  // save camera as "user pointer" to retrieve later by reference
  glfwSetWindowUserPointer(window, &camera);

  // callbacks
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSetCursorPosCallback(window, cursorPosCallback);
  glfwSetScrollCallback(window, scrollCallback);

  // hides the cursor and captures it (makes it stay in the center)
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cout << "Failed to initialize GLAD.\n";
    glfwTerminate();
    return -1;
  }

  // configure global opengl state
  glEnable(GL_DEPTH_TEST);

  Shader cubeShader{
      (std::string(CHAPTER_DIR) + "/shaders/cube_vertex.glsl").c_str(),
      (std::string(CHAPTER_DIR) + "/shaders/cube_fragment.glsl").c_str()};
  Shader lampShader{
      (std::string(CHAPTER_DIR) + "/shaders/lamp_vertex.glsl").c_str(),
      (std::string(CHAPTER_DIR) + "/shaders/lamp_fragment.glsl").c_str()};

  // reminder: sizeof(float) == 4 bytes
  // 1. object's VAO (and VBO) config
  unsigned int VBO{};
  unsigned int cubeVAO{};

  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, data::vertexData.size() * sizeof(float),
               data::vertexData.data(), GL_STATIC_DRAW);

  glBindVertexArray(cubeVAO);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0);

  // normal vector attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // texture coords attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void*>(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // diffuse map loading
  unsigned int diffuseMap{loadTexture("resources/box_diffuse.png")};
  // specular map laoding
  unsigned int specularMap{loadTexture("resources/box_specular.png")};

  cubeShader.use();
  cubeShader.setInt("material.diffuse", 0);
  cubeShader.setInt("material.specular", 1);

  // 2. lamp's VAO config
  unsigned int lampVAO{};

  glGenVertexArrays(1, &lampVAO);
  glBindVertexArray(lampVAO);

  // no need to bind it again, but we do it again for educational purposes
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0);

  while (!glfwWindowShouldClose(window)) {
    // deltaTime calculation to keep consistent the camera speed
    timing::currentFrame = static_cast<float>(glfwGetTime());
    timing::deltaTime = timing::currentFrame - timing::lastFrame;
    timing::lastFrame = timing::currentFrame;

    processInput(window);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // light properties
    glm::vec3 lightPosition{1.2f, 1.0f, 2.0f};

    // cube object
    cubeShader.use();

    cubeShader.setVec3("light.position", lightPosition);
    cubeShader.setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    cubeShader.setVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
    cubeShader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

    cubeShader.setFloat("material.shininess", 32.0f);

    cubeShader.setVec3("viewPosition", camera.getPosition());

    // model, view and projection matrices
    glm::mat4 cubeProjection{glm::perspective(
        glm::radians(camera.getFov()), window::aspectRatio, 0.1f, 100.0f)};

    cubeShader.setMat4("projection", cubeProjection);
    cubeShader.setMat4("view", camera.getViewMatrix());

    glm::mat4 cubeModel{glm::mat4(1.0)};
    cubeShader.setMat4("model", cubeModel);

    // bind diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);

    // bind specular map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);

    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // lamp object (light source)
    lampShader.use();

    // model, view and projection matrices
    glm::mat4 lampProjection{glm::perspective(
        glm::radians(camera.getFov()), window::aspectRatio, 0.1f, 100.0f)};

    lampShader.setMat4("projection", lampProjection);
    lampShader.setMat4("view", camera.getViewMatrix());

    glm::mat4 lampModel{glm::mat4(1.0)};
    lampModel = glm::translate(lampModel, lightPosition);
    lampModel = glm::scale(lampModel, glm::vec3(0.2f));

    lampShader.setMat4("model", lampModel);

    glBindVertexArray(lampVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteBuffers(1, &VBO);
  glDeleteVertexArrays(1, &cubeVAO);
  glDeleteVertexArrays(1, &lampVAO);

  cubeShader.remove();
  lampShader.remove();

  glfwTerminate();
  return 0;
}
