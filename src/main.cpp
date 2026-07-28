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

#include "callbacks.h"
#include "camera.h"
#include "constants.h"
#include "data.h"
#include "shader.h"
#include "textures.h"

// Light configuration
// clang-format off
namespace lights {
constexpr std::array<glm::vec3, 4> positions{
  glm::vec3(-3.0f, 0.0f, 0.0f),
  glm::vec3(-1.0f, 0.0f, 0.0f),
  glm::vec3( 1.0f, 0.0f, 0.0f),
  glm::vec3( 3.0f, 0.0f, 0.0f)
};
constexpr std::array<glm::vec3, 4> colors{
  glm::vec3(0.25f),
  glm::vec3(0.50f),
  glm::vec3(0.75f),
  glm::vec3(1.00f)
};
}  // namespace lights
// clang-format on

// Gamma correction configuration
namespace gammaCorrection {
inline bool isGammaEnable{false};
inline bool isGammaKeyPressed{false};
}  // namespace gammaCorrection

// `deltaTime` calculation to keep consistent the camera speed
void stabilizeFrame() {
  using namespace timing;
  currentFrame = static_cast<float>(glfwGetTime());
  deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;
}

// Process input
void processInput(GLFWwindow* window) {
  auto& camera{*static_cast<Camera*>(glfwGetWindowUserPointer(window))};

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS &&
      !gammaCorrection::isGammaKeyPressed) {
    gammaCorrection::isGammaEnable = !gammaCorrection::isGammaEnable;
    gammaCorrection::isGammaKeyPressed = true;
  }
  if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE) {
    gammaCorrection::isGammaKeyPressed = false;
  }

  camera.processKeyboardInput(window);
}

int main(int, char**) {
  // glfw: Initialization
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW.\n";
    return -1;
  }

  // glfw: Configuration
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window{glfwCreateWindow(window::width, window::height,
                                      "LearnOpenGL", nullptr, nullptr)};

  if (!window) {
    std::cout << "Failed to create GLFW window.\n";
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  // glad: Initialization
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cout << "Failed to initialize GLAD.\n";
    glfwTerminate();
    return -1;
  }

  // Hides the cursor and captures it (makes it stay in the center)
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // glfw callbacks
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSetCursorPosCallback(window, cursorPosCallback);
  glfwSetScrollCallback(window, scrollCallback);

  // Configure global OpenGL state
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Create camera and save it as "user pointer" to
  // retrieve later by reference
  Camera camera{glm::vec3(0.0f, 0.0f, 3.0f)};
  glfwSetWindowUserPointer(window, &camera);

  // Build and compile shaders
  Shader ourShader{"shaders/vert.glsl", "shaders/frag.glsl"};

  // Setup VAO (and VBO)
  unsigned int planeVBO{};
  unsigned int planeVAO{};

  glGenVertexArrays(1, &planeVAO);
  glGenBuffers(1, &planeVBO);

  glBindVertexArray(planeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
  glBufferData(GL_ARRAY_BUFFER, data::planeVertices.size() * sizeof(float),
               data::planeVertices.data(), GL_STATIC_DRAW);

  constexpr std::size_t stride{8 * sizeof(float)};

  // Position attribute
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                        reinterpret_cast<void*>(0));

  // Normal attribute
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                        reinterpret_cast<void*>(3 * sizeof(float)));

  // TexCoords attribute
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
                        reinterpret_cast<void*>(6 * sizeof(float)));

  glBindVertexArray(0);

  // Load textures
  unsigned int floorTex{loadTexture("assets/textures/wood.png", false)};
  unsigned int floorTexGammaCorrected{
      loadTexture("assets/textures/wood.png", true)};

  ourShader.use();
  ourShader.setInt("u_FloorTexture", 0);

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    stabilizeFrame();
    processInput(window);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ourShader.use();
    // Mode, view and projection matrices
    glm::mat4 projection{
        glm::perspective(glm::radians(camera.getFov()), window::aspectRatio,
                         constants::nearPlane, constants::farPlane)};

    ourShader.setMat4("u_Projection", projection);
    ourShader.setMat4("u_View", camera.getViewMatrix());

    // Set uniforms
    glUniform3fv(glGetUniformLocation(ourShader.getShaderProgramId(),
                                      "u_LightPositions"),
                 lights::positions.size(), &lights::positions[0][0]);
    glUniform3fv(
        glGetUniformLocation(ourShader.getShaderProgramId(), "u_LightColors"),
        lights::colors.size(), &lights::colors[0][0]);

    ourShader.setVec3("u_ViewPos", camera.getPosition());
    ourShader.setBool("u_isGammaEnable", gammaCorrection::isGammaEnable);

    // Check gamma correction state
    std::cout << "Using: "
              << (gammaCorrection::isGammaEnable ? "Gamma Correction Enabled"
                                                 : "Gamma Correction Disabled")
              << '\n';

    // Render plane
    glBindVertexArray(planeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gammaCorrection::isGammaEnable
                                     ? floorTexGammaCorrected
                                     : floorTex);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteBuffers(1, &planeVBO);
  glDeleteVertexArrays(1, &planeVAO);

  ourShader.remove();
  glfwTerminate();
  return 0;
}
