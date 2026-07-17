// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <stb_image/stb_image.h>

#include <cstddef>
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

  // build and compile shaders
  Shader cubeShader{
      (std::string(CHAPTER_DIR) + "/shaders/cube_vertex.glsl").c_str(),
      (std::string(CHAPTER_DIR) + "/shaders/cube_fragment.glsl").c_str()};
  Shader skyboxShader{
      (std::string(CHAPTER_DIR) + "/shaders/skybox_vertex.glsl").c_str(),
      (std::string(CHAPTER_DIR) + "/shaders/skybox_fragment.glsl").c_str()};

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

  constexpr std::size_t cubeStride{6 * sizeof(float)};

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, cubeStride,
                        reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0);

  // normal vectors attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, cubeStride,
                        reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  // 2. skybox's VAO (and VBO) config
  unsigned int skyboxVBO{};
  unsigned int skyboxVAO{};

  glGenVertexArrays(1, &skyboxVAO);
  glGenBuffers(1, &skyboxVBO);
  glBindVertexArray(skyboxVAO);

  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glBufferData(GL_ARRAY_BUFFER, data::skyboxVertices.size() * sizeof(float),
               data::skyboxVertices.data(), GL_STATIC_DRAW);

  constexpr std::size_t skyboxStride{3 * sizeof(float)};

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, skyboxStride,
                        reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);

  // cubemap
  const std::array<std::string, 6> cubemapFacesPaths{
      "resources/textures/skybox/right.jpg",
      "resources/textures/skybox/left.jpg",
      "resources/textures/skybox/top.jpg",
      "resources/textures/skybox/bottom.jpg",
      "resources/textures/skybox/front.jpg",
      "resources/textures/skybox/back.jpg"};

  unsigned int cubemapTexture{loadCubemap(cubemapFacesPaths)};

  cubeShader.use();
  cubeShader.setInt("u_SkyboxTexture", 0);

  skyboxShader.use();
  skyboxShader.setInt("u_SkyboxTexture", 0);

  while (!glfwWindowShouldClose(window)) {
    stabilizeFrame();
    processInput(window);

    // Make sure we clear the framebuffer's content
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Setting view and projection matrices
    glm::mat4 projection{glm::perspective(glm::radians(camera.getFov()),
                                          window::aspectRatio, 0.1f, 100.0f)};

    // Setting uniforms
    cubeShader.use();
    cubeShader.setVec3("u_CameraPosition", camera.getPosition());

    cubeShader.setMat4("u_Projection", projection);
    cubeShader.setMat4("u_View", camera.getViewMatrix());

    // 1. Cube
    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    cubeShader.setMat4("u_Model", glm::mat4(1.0));

    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // 2. Skybox
    // Change depth function so depth test passes when values
    // are equal to depth buffer's content
    glDepthFunc(GL_LEQUAL);

    skyboxShader.use();
    skyboxShader.setMat4("u_Projection", projection);
    skyboxShader.setMat4("u_View",
                         glm::mat4(glm::mat3(camera.getViewMatrix())));
    // skyboxShader.setMat4("u_View", camera.getViewMatrix());

    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // Set depth function back to default
    glDepthFunc(GL_LESS);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &cubeVAO);
  glDeleteVertexArrays(1, &skyboxVAO);
  glDeleteBuffers(1, &cubeVBO);
  glDeleteBuffers(1, &skyboxVBO);

  glfwTerminate();
  return 0;
}
