// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <array>
#include <cmath>
#include <cstddef>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "callbacks.h"
#include "camera.h"
#include "constants.h"
#include "data.h"
#include "render.h"
#include "shader.h"
#include "textures.h"

namespace settings {
inline float heightScale{0.1f};
}  // namespace settings

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

  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
    if (settings::heightScale > 0.0f) {
      settings::heightScale -= 0.0005f;
    } else {
      settings::heightScale = 0.0f;
    }
  } else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    if (settings::heightScale < 1.0f) {
      settings::heightScale += 0.0005f;
    } else {
      settings::heightScale = 1.0f;
    }
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

  // Create camera and save it as "user pointer" to
  // retrieve later by reference
  Camera camera{glm::vec3(0.0f, 0.0f, 3.0f)};
  glfwSetWindowUserPointer(window, &camera);

  // Build and compile shaders
  Shader ourShader{"shaders/vert.glsl", "shaders/frag.glsl"};

  // Load textures
  unsigned int diffuseMapId{loadTexture("assets/textures/wood.png")};
  unsigned int normalMapId{loadTexture("assets/textures/toy_box_normal.png")};
  unsigned int heightMapId{loadTexture("assets/textures/toy_box_disp.png")};

  // Shaders configuration
  ourShader.use();
  ourShader.setInt("u_DiffuseMap", 0);
  ourShader.setInt("u_NormalMap", 1);
  ourShader.setInt("u_DepthMap", 2);

  // Light configuration
  glm::vec3 lightPosition{glm::vec3(0.5f, 1.0f, 0.3f)};

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    stabilizeFrame();
    processInput(window);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Configure view/projection matrices
    glm::mat4 projection{glm::perspective(glm::radians(camera.getFov()),
                                          window::aspectRatio, 0.1f, 100.0f)};
    glm::mat4 view{camera.getViewMatrix()};

    // and set uniforms
    ourShader.use();
    ourShader.setMat4("u_Projection", projection);
    ourShader.setMat4("u_View", view);

    // Render parallax-mapped quad
    glm::mat4 model{glm::mat4(1.0f)};
    // Rotate the quad to show parallax mapping from multiple directions
    model = glm::rotate(
        model, glm::radians(static_cast<float>(glfwGetTime()) * -10.0f),
        glm::normalize(glm::vec3(1.0, 0.0, 1.0)));

    ourShader.setVec3("u_ViewPos", camera.getPosition());
    ourShader.setVec3("u_LightPos", lightPosition);
    ourShader.setMat4("u_Model", model);
    ourShader.setFloat("u_HeightScale", settings::heightScale);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMapId);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalMapId);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, heightMapId);

    renderQuad();

    // Render light source:
    // Simply re-renders a smaller plane at the light's position
    // for debugging/visualization
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPosition);
    model = glm::scale(model, glm::vec3(0.1f));
    ourShader.setMat4("u_Model", model);

    renderQuad();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  ourShader.remove();

  glfwTerminate();
  return 0;
}
