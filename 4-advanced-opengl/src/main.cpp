// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>

#include "camera.h"
#include "constants.h"
#include "shader.h"

// clang-format off
namespace window {
// Fullscreen may cause bugs, disabled by default
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

namespace data {
constexpr std::array<float, 5 * 3 * 2> quadVertices{
  // positions     // colors
  -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
   0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
  -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

  -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
   0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
   0.05f,  0.05f,  0.0f, 1.0f, 1.0f
};
} // namespace data
// clang-format on

// GLFW callbacks
void framebufferSizeCallback([[maybe_unused]] GLFWwindow* window, int width,
                             int height) {
  glViewport(0, 0, width, height);
}

// Mouse/cursor controls
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

// `offsetY` means how much we scrolled vertically
void scrollCallback([[maybe_unused]] GLFWwindow* window,
                    [[maybe_unused]] double offsetX, double offsetY) {
  auto& camera{*static_cast<Camera*>(glfwGetWindowUserPointer(window))};
  camera.processScrollInput(offsetY);
}

// `deltaTime` calculation to keep consistent the camera speed
void stabilizeFrame() {
  timing::currentFrame = static_cast<float>(glfwGetTime());
  timing::deltaTime = timing::currentFrame - timing::lastFrame;
  timing::lastFrame = timing::currentFrame;
}

// Process input
void processInput(GLFWwindow* window) {
  auto& camera{*static_cast<Camera*>(glfwGetWindowUserPointer(window))};

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  camera.processKeyboardInput(window);
}

int main(int, char**) {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW.\n";
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window{glfwCreateWindow(
      window::width, window::height, "LearnOpenGL",
      window::openFullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr)};

  if (!window) {
    std::cout << "Failed to create GLFW window.\n";
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  // Hides the cursor and captures it (makes it stay in the center)
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Create camera and save it as "user pointer" to retrieve later by reference
  Camera camera{glm::vec3(0.0f, 0.0f, 3.0f)};
  glfwSetWindowUserPointer(window, &camera);

  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSetCursorPosCallback(window, cursorPosCallback);
  glfwSetScrollCallback(window, scrollCallback);

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cout << "Failed to initialize GLAD.\n";
    glfwTerminate();
    return -1;
  }

  // Configure global OpenGL state
  glEnable(GL_DEPTH_TEST);

  // Build and compile shaders
  Shader ourShader{
      (std::string(CHAPTER_DIR) + "/shaders/vertex.glsl").c_str(),
      (std::string(CHAPTER_DIR) + "/shaders/fragment.glsl").c_str()};

  unsigned int quadVAO{};
  unsigned int quadVBO{};

  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);

  glBindVertexArray(quadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, data::quadVertices.size() * sizeof(float),
               data::quadVertices.data(), GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        reinterpret_cast<void*>(0));

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        reinterpret_cast<void*>(2 * sizeof(float)));

  // Create offsets manually
  std::array<glm::vec2, 100> translations{};
  std::size_t index{};
  float offset{0.1f};

  for (int y{-10}; y < 10; y += 2) {
    for (int x{-10}; x < 10; x += 2) {
      translations[index] = glm::vec2{static_cast<float>(x) / 10.0f + offset,
                                      static_cast<float>(y) / 10.0f + offset};
      index++;
    }
  }

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    stabilizeFrame();
    processInput(window);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Don't forget to enable shader before setting uniforms
    ourShader.use();
    for (std::size_t i{0}; i < 100; ++i) {
      ourShader.setVec2("u_Offsets[" + std::to_string(i) + ']',
                        translations[i]);
    }

    glBindVertexArray(quadVAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);
    glBindVertexArray(0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &quadVAO);
  glDeleteBuffers(1, &quadVBO);

  glfwTerminate();
  return 0;
}
