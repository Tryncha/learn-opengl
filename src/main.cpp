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
#include "model.h"
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

  // Create camera and save it as "user pointer" to
  // retrieve later by reference
  Camera camera{glm::vec3(0.0f, 0.0f, 5.0f)};
  glfwSetWindowUserPointer(window, &camera);

  // Build and compile shaders
  Shader geometryShader{"shaders/geometry.vert", "shaders/geometry.frag"};
  Shader lightingShader{"shaders/lighting.vert", "shaders/lighting.frag"};
  Shader lightBoxShader{"shaders/lightbox.vert", "shaders/lightbox.frag"};

  // Load model
  Model backpackModel{"assets/objects/backpack/backpack.obj"};

  // Configure geometry framebuffer
  unsigned int gFramebufferId{};
  glGenFramebuffers(1, &gFramebufferId);
  glBindFramebuffer(GL_FRAMEBUFFER, gFramebufferId);

  // Position color buffer
  unsigned int gPositionTextureId{};
  glGenTextures(1, &gPositionTextureId);
  glBindTexture(GL_TEXTURE_2D, gPositionTextureId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window::width, window::height, 0,
               GL_RGBA, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         gPositionTextureId, 0);

  // Normal color buffer
  unsigned int gNormalTextureId{};
  glGenTextures(1, &gNormalTextureId);
  glBindTexture(GL_TEXTURE_2D, gNormalTextureId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window::width, window::height, 0,
               GL_RGBA, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                         gNormalTextureId, 0);

  // Albedo + specular color buffers
  unsigned int gAlbedoSpecTextureId{};
  glGenTextures(1, &gAlbedoSpecTextureId);
  glBindTexture(GL_TEXTURE_2D, gAlbedoSpecTextureId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window::width, window::height, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
                         gAlbedoSpecTextureId, 0);

  // Tell OpenGL which color attachments we'll use
  // (of this framebuffer) for rendering
  std::array<unsigned int, 3> attachments{
      GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
  glDrawBuffers(3, attachments.data());

  // Create and attach depth buffer (renderbuffer)
  unsigned int depthRenderbufferId;
  glGenRenderbuffers(1, &depthRenderbufferId);
  glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbufferId);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window::width,
                        window::height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, depthRenderbufferId);

  // Finally check if framebuffer is complete
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "Framebuffer not complete!\n";
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Light configuration
  const std::array<glm::vec3, 9> objectPositions{
      glm::vec3(-3.0, -0.5, -3.0), glm::vec3(0.0, -0.5, -3.0),
      glm::vec3(3.0, -0.5, -3.0),  glm::vec3(-3.0, -0.5, 0.0),
      glm::vec3(0.0, -0.5, 0.0),   glm::vec3(3.0, -0.5, 0.0),
      glm::vec3(-3.0, -0.5, 3.0),  glm::vec3(0.0, -0.5, 3.0),
      glm::vec3(3.0, -0.5, 3.0)};

  constexpr int N_LIGHTS{32};
  std::array<glm::vec3, N_LIGHTS> lightPositions{};
  std::array<glm::vec3, N_LIGHTS> lightColors{};
  srand(13);

  for (std::size_t i{0}; i < N_LIGHTS; ++i) {
    // Calculate slightly random offsets
    float xPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
    float yPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 4.0);
    float zPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
    lightPositions[i] = glm::vec3(xPos, yPos, zPos);

    // also calculate random colors, all between 0.5 and 1.0
    float rColor = static_cast<float>(((rand() % 100) / 200.0) + 0.5f);
    float gColor = static_cast<float>(((rand() % 100) / 200.0) + 0.5f);
    float bColor = static_cast<float>(((rand() % 100) / 200.0) + 0.5f);
    lightColors[i] = glm::vec3(rColor, gColor, bColor);
  }

  // Shader configuration
  lightingShader.use();
  lightingShader.setInt("u_gPosition", 0);
  lightingShader.setInt("u_gNormal", 1);
  lightingShader.setInt("u_gAlbedoSpec", 2);

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    stabilizeFrame();
    processInput(window);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 1. Geometry Pass:
    // Render the scene's geometry/color data into gbuffer
    glBindFramebuffer(GL_FRAMEBUFFER, gFramebufferId);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Configure view/projection matrices
    glm::mat4 projection{glm::perspective(glm::radians(camera.getFov()),
                                          window::aspectRatio, 0.1f, 100.0f)};
    glm::mat4 view{camera.getViewMatrix()};
    glm::mat4 model{glm::mat4(1.0f)};

    geometryShader.use();
    geometryShader.setMat4("u_Projection", projection);
    geometryShader.setMat4("u_View", view);

    for (const auto& pos : objectPositions) {
      model = glm::mat4(1.0f);
      model = glm::translate(model, pos);
      model = glm::scale(model, glm::vec3(0.5f));

      geometryShader.setMat4("u_Model", model);
      backpackModel.draw(geometryShader);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 2. Lighting Pass
    // Calculate lighting by iterating over a screen filled
    // quad pixel-by-pixel using the gbuffer's content
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    lightingShader.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPositionTextureId);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormalTextureId);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpecTextureId);

    // Note that we don't send this to the shader.
    // In our case, we assume it is always 1.0
    constexpr float constant{1.0f};

    constexpr float linear{0.7f};
    constexpr float quadratic{1.8f};

    // Send light relevant uniforms
    for (std::size_t i{0}; i < lightPositions.size(); ++i) {
      lightingShader.setVec3("u_Lights[" + std::to_string(i) + "].position",
                             lightPositions[i]);
      lightingShader.setVec3("u_Lights[" + std::to_string(i) + "].color",
                             lightColors[i]);

      // Update attenuation parameters and calculate radius
      lightingShader.setFloat("u_Lights[" + std::to_string(i) + "].linear",
                              linear);
      lightingShader.setFloat("u_Lights[" + std::to_string(i) + "].quadratic",
                              quadratic);

      // Then calculate radius of light volume/sphere
      const float maxBrightness = std::fmaxf(
          std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
      float radius =
          (-linear +
           std::sqrt(linear * linear -
                     4 * quadratic *
                         (constant - (256.0f / 5.0f) * maxBrightness))) /
          (2.0f * quadratic);

      lightingShader.setFloat("u_Lights[" + std::to_string(i) + "].radius",
                              radius);
    }

    lightingShader.setVec3("u_ViewPos", camera.getPosition());

    // Finally render quad
    renderQuad();

    // 2.5 Copy content of geometry's depth buffer to
    // default framebuffer's depth buffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gFramebufferId);
    // Write to default framebuffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    // Blit to default framebuffer
    glBlitFramebuffer(0, 0, window::width, window::height, 0, 0, window::width,
                      window::height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 3. Render lights on top of scene
    lightBoxShader.use();
    lightBoxShader.setMat4("u_Projection", projection);
    lightBoxShader.setMat4("u_View", view);

    for (std::size_t i{0}; i < lightPositions.size(); ++i) {
      model = glm::mat4(1.0f);
      model = glm::translate(model, lightPositions[i]);
      model = glm::scale(model, glm::vec3(0.125f));

      lightBoxShader.setMat4("u_Model", model);
      lightBoxShader.setVec3("u_LightColor", lightColors[i]);
      renderCube();
    }

    // glfw: swap buffers and poll IO events
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  geometryShader.remove();
  lightingShader.remove();
  lightBoxShader.remove();

  glfwTerminate();
  return 0;
}
