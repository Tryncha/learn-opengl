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
  glEnable(GL_CULL_FACE);

  // Create camera and save it as "user pointer" to
  // retrieve later by reference
  Camera camera{glm::vec3(0.0f, 0.0f, 3.0f)};
  glfwSetWindowUserPointer(window, &camera);

  // Build and compile shaders
  Shader ourShader{"shaders/vert.glsl", "shaders/frag.glsl"};
  Shader depthShader{"shaders/depth_vert.glsl", "shaders/depth_frag.glsl",
                     "shaders/depth_geom.glsl"};

  // Load textures
  unsigned int woodTexture{loadTexture("assets/textures/wood.png", false)};

  // Configure depth map framebuffer
  constexpr int shadowWidth{1024};
  constexpr int shadowHeight{1024};

  // Create depth map FBO
  unsigned int depthMapFramebufferId{};
  glGenFramebuffers(1, &depthMapFramebufferId);

  // Create depth cubemap texture
  unsigned int depthCubemapId{};
  glGenTextures(1, &depthCubemapId);
  glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemapId);

  for (int i{0}; i < 6; ++i) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                 shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                 nullptr);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  // Attach depth texture as FBO's depth buffer
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFramebufferId);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemapId, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Shaders configuration
  ourShader.use();
  ourShader.setInt("u_DiffuseTexture", 0);
  ourShader.setInt("u_ShadowMap", 1);

  // Light configuration
  glm::vec3 lightPosition{glm::vec3(0.0f)};

  // Cubemap transformation matrices
  constexpr float nearPlane{1.0f};
  constexpr float farPlane{25.0f};

  const glm::mat4 shadowProjection{glm::perspective(
      glm::radians(90.0f),
      static_cast<float>(shadowWidth) / static_cast<float>(shadowHeight),
      nearPlane, farPlane)};

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    stabilizeFrame();
    processInput(window);

    // Change light position over time
    lightPosition.z = 3.0f * static_cast<float>(0.5f * std::sin(glfwGetTime()));

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Recalculate transformation matrices
    const std::array<glm::mat4, 6> shadowTransf{
        glm::mat4(shadowProjection *
                  glm::lookAt(lightPosition,
                              lightPosition + glm::vec3(1.0f, 0.0f, 0.0f),
                              glm::vec3(0.0f, -1.0f, 0.0f))),
        glm::mat4(shadowProjection *
                  glm::lookAt(lightPosition,
                              lightPosition + glm::vec3(-1.0f, 0.0f, 0.0f),
                              glm::vec3(0.0f, -1.0f, 0.0f))),
        glm::mat4(shadowProjection *
                  glm::lookAt(lightPosition,
                              lightPosition + glm::vec3(0.0f, 1.0f, 0.0f),
                              glm::vec3(0.0f, 0.0f, 1.0f))),
        glm::mat4(shadowProjection *
                  glm::lookAt(lightPosition,
                              lightPosition + glm::vec3(0.0f, -1.0f, 0.0f),
                              glm::vec3(0.0f, 0.0f, -1.0f))),
        glm::mat4(shadowProjection *
                  glm::lookAt(lightPosition,
                              lightPosition + glm::vec3(0.0f, 0.0f, 1.0f),
                              glm::vec3(0.0f, -1.0f, 0.0f))),
        glm::mat4(shadowProjection *
                  glm::lookAt(lightPosition,
                              lightPosition + glm::vec3(0.0f, 0.0f, -1.0f),
                              glm::vec3(0.0f, -1.0f, 0.0f)))};

    // 1. Render scene to depth cubemap
    glViewport(0, 0, shadowWidth, shadowHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFramebufferId);
    glClear(GL_DEPTH_BUFFER_BIT);

    depthShader.use();

    for (std::size_t i{0}; i < 6; ++i) {
      depthShader.setMat4("u_ShadowTransf[" + std::to_string(i) + "]",
                          shadowTransf[i]);
    }

    depthShader.setFloat("u_FarPlane", farPlane);
    depthShader.setVec3("u_LightPos", lightPosition);

    renderScene(depthShader);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 2. Render scene as normal using the generated depth/shadow map
    glViewport(0, 0, window::width, window::height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ourShader.use();

    glm::mat4 projection{glm::perspective(glm::radians(camera.getFov()),
                                          window::aspectRatio, 0.1f, 100.0f)};
    glm::mat4 view{camera.getViewMatrix()};

    // Set transformation matrices uniforms
    ourShader.setMat4("u_Projection", projection);
    ourShader.setMat4("u_View", view);

    // Set light uniforms
    ourShader.setVec3("u_ViewPos", camera.getPosition());
    ourShader.setVec3("u_LightPos", lightPosition);
    ourShader.setFloat("u_FarPlane", farPlane);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemapId);

    renderScene(ourShader);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  ourShader.remove();
  depthShader.remove();

  glfwTerminate();
  return 0;
}
