// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <array>
#include <cstddef>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>

#include "camera.h"
#include "constants.h"
#include "model.h"
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

float genRandomDisplacement(float offset) {
  return static_cast<float>(rand() % static_cast<int>(2 * offset * 100)) /
             100.0f -
         offset;
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

  // Load models
  Model planetModel{"resources/objects/planet/planet.obj"};
  Model rockModel{"resources/objects/rock/rock.obj"};

  // Preparing the scene
  std::size_t rocksAmount{1000};
  float radius{50.0f};

  glm::mat4* modelMatrices{new glm::mat4[rocksAmount]};

  // Initialize random seed
  srand(static_cast<unsigned int>(glfwGetTime()));

  for (std::size_t i{0}; i < rocksAmount; ++i) {
    glm::mat4 model{glm::mat4(1.0)};

    // 1. Translation:
    // Displace along circle with 'radius' in range [-offset, offset]
    float angle{static_cast<float>(i) / static_cast<float>(rocksAmount) *
                360.0f};

    // Keep height of field smaller compared to width of x and z
    float x{std::sin(angle) * radius + genRandomDisplacement(2.5f)};
    float y{genRandomDisplacement(2.5f) * 0.4f};
    float z{std::cos(angle) * radius + genRandomDisplacement(2.5f)};

    model = glm::translate(model, glm::vec3(x, y, z));

    // 2. Scale between 0.05 and 0.25
    float scaleFactor{static_cast<float>(rand() % 20) / 100.0f + 0.05f};
    model = glm::scale(model, glm::vec3(scaleFactor));

    // 3. Add random rotation around a semi-randomly
    // picked rotation axis vector
    float rotAngle{static_cast<float>(rand() % 360)};
    model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

    // 4. Add to list of matrices
    modelMatrices[i] = model;
  }

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    stabilizeFrame();
    processInput(window);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Don't forget to enable shader before setting uniforms
    ourShader.use();

    // View and projection matrices
    glm::mat4 projection{glm::perspective(glm::radians(camera.getFov()),
                                          window::aspectRatio, 0.1f, 100.0f)};

    ourShader.setMat4("u_Projection", projection);
    ourShader.setMat4("u_View", camera.getViewMatrix());

    // Planet
    glm::mat4 model{glm::mat4(1.0)};
    model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
    model = glm::scale(model, glm::vec3(4.0f));

    ourShader.setMat4("u_Model", model);

    planetModel.draw(ourShader);

    // Rocks
    for (std::size_t i{0}; i < rocksAmount; ++i) {
      ourShader.setMat4("u_Model", modelMatrices[i]);
      rockModel.draw(ourShader);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
