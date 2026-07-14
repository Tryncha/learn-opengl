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
#include <map>
#include <ranges>
#include <string>

#include "camera.h"
#include "constants.h"
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

  // Configure global opengl state
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // build and compile shaders
  Shader ourShader{
      (std::string(CHAPTER_DIR) + "/shaders/vertex.glsl").c_str(),
      (std::string(CHAPTER_DIR) + "/shaders/fragment.glsl").c_str()};

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

  constexpr std::size_t cubeStride{5 * sizeof(float)};

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, cubeStride,
                        reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0);

  // texture coords attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, cubeStride,
                        reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  // 2. plane's VAO (and VBO) config
  unsigned int planeVBO{};
  unsigned int planeVAO{};

  glGenVertexArrays(1, &planeVAO);
  glGenBuffers(1, &planeVBO);
  glBindVertexArray(planeVAO);

  glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
  glBufferData(GL_ARRAY_BUFFER, data::planeVertices.size() * sizeof(float),
               data::planeVertices.data(), GL_STATIC_DRAW);

  constexpr std::size_t planeStride{5 * sizeof(float)};

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, planeStride,
                        reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0);

  // texture coords attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, planeStride,
                        reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  // 3. Transparent's VAO (and VBO) config
  unsigned int transparentVBO{};
  unsigned int transparentVAO{};

  glGenVertexArrays(1, &transparentVAO);
  glGenBuffers(1, &transparentVBO);
  glBindVertexArray(transparentVAO);

  glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
  glBufferData(GL_ARRAY_BUFFER,
               data::transparentVertices.size() * sizeof(float),
               data::transparentVertices.data(), GL_STATIC_DRAW);

  constexpr std::size_t transparentStride{5 * sizeof(float)};

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, transparentStride,
                        reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0);

  // texture coords attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, transparentStride,
                        reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  // textures
  unsigned int cubeTexture{loadTexture("resources/textures/marble.jpg")};
  unsigned int planeTexture{loadTexture("resources/textures/metal.png")};
  unsigned int transparentTexture{loadTexture("resources/textures/window.png")};

  ourShader.setInt("u_Texture1", 0);

  while (!glfwWindowShouldClose(window)) {
    stabilizeFrame();
    processInput(window);

    // Sort the windows before rendering
    std::map<float, glm::vec3> sorted{};
    for (std::size_t i{0}; i < data::windowPositions.size(); ++i) {
      float distance =
          glm::length(camera.getPosition() - data::windowPositions[i]);
      sorted[distance] = data::windowPositions[i];
    }

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Setting view and projection matrices
    glm::mat4 projection{glm::perspective(glm::radians(camera.getFov()),
                                          window::aspectRatio, 0.1f, 100.0f)};

    // Setting uniforms
    ourShader.use();

    ourShader.setMat4("u_Projection", projection);
    ourShader.setMat4("u_View", camera.getViewMatrix());

    // Model matrix
    // 1. Plane
    glBindVertexArray(planeVAO);
    glBindTexture(GL_TEXTURE_2D, planeTexture);

    glm::mat4 planeModel{glm::mat4(1.0)};
    // Move slightly down to prevent z-fighting
    planeModel = glm::translate(planeModel, glm::vec3(0.0f, -0.01f, 0.0f));
    ourShader.setMat4("u_Model", planeModel);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // 2. Cubes
    glBindVertexArray(cubeVAO);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);

    // First cube
    glm::mat4 modelCube1{glm::mat4(1.0)};
    modelCube1 = glm::translate(modelCube1, glm::vec3(-1.0f, 0.0f, -1.0f));
    ourShader.setMat4("u_Model", modelCube1);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    // Second cube
    glm::mat4 modelCube2{glm::mat4(1.0)};
    modelCube2 = glm::translate(modelCube2, glm::vec3(2.0f, 0.0f, 0.0f));
    ourShader.setMat4("u_Model", modelCube2);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    // 3. Windows (from furthest to nearest)
    glBindVertexArray(transparentVAO);
    glBindTexture(GL_TEXTURE_2D, transparentTexture);

    for (const auto& [distance, position] : std::views::reverse(sorted)) {
      glm::mat4 windowModel{glm::mat4(1.0)};
      windowModel = glm::translate(windowModel, position);
      ourShader.setMat4("u_Model", windowModel);
      glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &cubeVAO);
  glDeleteVertexArrays(1, &planeVAO);
  glDeleteVertexArrays(1, &transparentVAO);
  glDeleteBuffers(1, &cubeVBO);
  glDeleteBuffers(1, &planeVBO);
  glDeleteBuffers(1, &transparentVBO);

  glfwTerminate();
  return 0;
}
