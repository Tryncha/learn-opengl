// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <stb_image/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>

#include "camera.h"
#include "constants.h"
#include "data.h"
#include "shader.h"

// clang-format off
namespace window {
// fullscreen may cause bugs, disabled by default
constexpr bool openFullscreen{false};
constexpr int width {1280};
constexpr int height{720};
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

// deltaTime calculation to keep consistent the camera speed
void stabilizeFrame() {
  timing::currentFrame = static_cast<float>(glfwGetTime());
  timing::deltaTime = timing::currentFrame - timing::lastFrame;
  timing::lastFrame = timing::currentFrame;
}

// process input
void processInput(GLFWwindow* window) {
  auto& camera{*static_cast<Camera*>(glfwGetWindowUserPointer(window))};

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  camera.processKeyboardInput(window);
}

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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                    format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
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

unsigned int loadCubemap(const std::array<std::string, 6>& facesPaths) {
  int width{};
  int height{};
  int nrChannels{};

  unsigned int textureId{};
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

  for (std::size_t i{0}; i < facesPaths.size(); ++i) {
    unsigned char* textureData{
        stbi_load(facesPaths[i].c_str(), &width, &height, &nrChannels, 0)};

    if (textureData) {
      glTexImage2D(
          GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<unsigned int>(i), 0,
          GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
    } else {
      std::cout << "Cubemap texture failed to load at path: " << facesPaths[i]
                << '\n';
    }

    stbi_image_free(textureData);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

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
  // glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

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

  // Configure global opengl state
  glEnable(GL_DEPTH_TEST);

  // reminder: sizeof(float) == 4 bytes
  // 1. cube's VAO (and VBO) config
  unsigned int cubeVBO{};
  unsigned int cubeVAO{};

  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &cubeVBO);
  glBindVertexArray(cubeVAO);

  glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
  glBufferData(GL_ARRAY_BUFFER, data::cubeVertices.size() * sizeof(float),
               data::cubeVertices.data(), GL_STATIC_DRAW);

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                        reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);

  // build and compile shaders
  Shader redShader{
      (std::string(CHAPTER_DIR) + "/shaders/vrtx_cube.glsl").c_str(),
      (std::string(CHAPTER_DIR) + "/shaders/frag_red.glsl").c_str()};
  Shader greenShader{
      (std::string(CHAPTER_DIR) + "/shaders/vrtx_cube.glsl").c_str(),
      (std::string(CHAPTER_DIR) + "/shaders/frag_green.glsl").c_str()};
  Shader blueShader{
      (std::string(CHAPTER_DIR) + "/shaders/vrtx_cube.glsl").c_str(),
      (std::string(CHAPTER_DIR) + "/shaders/frag_blue.glsl").c_str()};
  Shader yellowShader{
      (std::string(CHAPTER_DIR) + "/shaders/vrtx_cube.glsl").c_str(),
      (std::string(CHAPTER_DIR) + "/shaders/frag_yellow.glsl").c_str()};

  redShader.use();
  redShader.setUniformBlock("ub_Matrices", 0);

  greenShader.use();
  greenShader.setUniformBlock("ub_Matrices", 0);

  blueShader.use();
  blueShader.setUniformBlock("ub_Matrices", 0);

  yellowShader.use();
  yellowShader.setUniformBlock("ub_Matrices", 0);

  // Uniform buffer objects
  unsigned int matricesUBO{};
  glGenBuffers(1, &matricesUBO);

  glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
  glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr,
               GL_STATIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  glBindBufferRange(GL_UNIFORM_BUFFER, 0, matricesUBO, 0,
                    2 * sizeof(glm::mat4));

  while (!glfwWindowShouldClose(window)) {
    stabilizeFrame();
    processInput(window);

    // Make sure we clear the framebuffer's content
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Projection matrix
    float fov{glm::radians(camera.getFov())};
    glm::mat4 projection{
        glm::perspective(fov, window::aspectRatio, 0.1f, 100.0f)};

    glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4),
                    glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // View matrix
    glm::mat4 view{camera.getViewMatrix()};

    glBindBuffer(GL_UNIFORM_BUFFER, matricesUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
                    glm::value_ptr(view));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Red cube, top-left
    glm::mat4 redModel{glm::mat4(1.0)};
    redModel = glm::translate(redModel, glm::vec3(-0.75f, 0.75f, 0.0f));

    redShader.use();
    redShader.setMat4("u_Model", redModel);

    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // Green cube, top-right
    glm::mat4 greenModel{glm::mat4(1.0)};
    greenModel = glm::translate(greenModel, glm::vec3(0.75f, 0.75f, 0.0f));

    greenShader.use();
    greenShader.setMat4("u_Model", greenModel);

    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // Blue cube, bottom-right
    glm::mat4 blueModel{glm::mat4(1.0)};
    blueModel = glm::translate(blueModel, glm::vec3(0.75f, -0.75f, 0.0f));

    blueShader.use();
    blueShader.setMat4("u_Model", blueModel);

    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // Yellow cube, bottom-left
    glm::mat4 yellowModel{glm::mat4(1.0)};
    yellowModel = glm::translate(yellowModel, glm::vec3(-0.75f, -0.75f, 0.0f));

    yellowShader.use();
    yellowShader.setMat4("u_Model", yellowModel);

    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &cubeVAO);
  glDeleteBuffers(1, &cubeVBO);
  glDeleteBuffers(1, &matricesUBO);

  glfwTerminate();
  return 0;
}
