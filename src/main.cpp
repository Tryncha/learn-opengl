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
#include <random>

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

float ourLerp(float a, float b, float f) { return a + f * (b - a); }

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
  Shader ssaoShader{"shaders/ssao.vert", "shaders/ssao.frag"};
  Shader ssaoBlurShader{"shaders/ssao_blur.vert", "shaders/ssao_blur.frag"};

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
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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

  // Albedo color buffer
  unsigned int gAlbedoTextureId{};
  glGenTextures(1, &gAlbedoTextureId);
  glBindTexture(GL_TEXTURE_2D, gAlbedoTextureId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window::width, window::height, 0,
               GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
                         gAlbedoTextureId, 0);

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

  // Also create framebuffer to hold SSAO processing stage
  unsigned int ssaoFramebufferId{};
  glGenFramebuffers(1, &ssaoFramebufferId);
  glBindFramebuffer(GL_FRAMEBUFFER, ssaoFramebufferId);

  // SSAO color buffer
  unsigned int ssaoColorBufferId{};
  glGenTextures(1, &ssaoColorBufferId);
  glBindTexture(GL_TEXTURE_2D, ssaoColorBufferId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, window::width, window::height, 0,
               GL_RED, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         ssaoColorBufferId, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "Framebuffer not complete!\n";
  }

  unsigned int ssaoBlurFramebufferId{};
  glGenFramebuffers(1, &ssaoBlurFramebufferId);

  // and blur stage
  unsigned int ssaoColorBufferBlurId{};
  glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFramebufferId);
  glGenTextures(1, &ssaoColorBufferBlurId);
  glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlurId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, window::width, window::height, 0,
               GL_RED, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         ssaoColorBufferBlurId, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "Framebuffer not complete!\n";
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Generate sample kernel
  // Generates random floats between 0.0 and 1.0
  std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
  std::default_random_engine generator{};
  std::array<glm::vec3, 64> ssaoKernel{};

  for (std::size_t i{0}; i < 64; ++i) {
    glm::vec3 sample{randomFloats(generator) * 2.0 - 1.0,
                     randomFloats(generator) * 2.0 - 1.0,
                     randomFloats(generator)};

    sample = glm::normalize(sample);
    sample *= randomFloats(generator);
    float scale = float(i) / 64.0f;

    // Scale samples s.t. they're more aligned to center of kernel
    scale = ourLerp(0.1f, 1.0f, scale * scale);
    sample *= scale;
    ssaoKernel[i] = sample;
  }

  // Generate noise texture
  std::array<glm::vec3, 16> ssaoNoise{};

  for (std::size_t i{0}; i < 16; ++i) {
    // Rotate around z-axis (in tangent space)
    glm::vec3 noise{randomFloats(generator) * 2.0 - 1.0,
                    randomFloats(generator) * 2.0 - 1.0, 0.0f};
    ssaoNoise[i] = noise;
  }

  unsigned int noiseTextureId{};
  glGenTextures(1, &noiseTextureId);
  glBindTexture(GL_TEXTURE_2D, noiseTextureId);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT,
               &ssaoNoise[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // Lighting info
  glm::vec3 lightPos{glm::vec3(2.0, 4.0, -2.0)};
  glm::vec3 lightColor{glm::vec3(0.2, 0.2, 0.7)};

  // Shader configuration
  lightingShader.use();
  lightingShader.setInt("u_gPosition", 0);
  lightingShader.setInt("u_gNormal", 1);
  lightingShader.setInt("u_gAlbedo", 2);
  lightingShader.setInt("u_SSAO", 3);

  ssaoShader.use();
  ssaoShader.setInt("u_gPosition", 0);
  ssaoShader.setInt("u_gNormal", 1);
  ssaoShader.setInt("u_TexNoise", 2);

  ssaoBlurShader.use();
  ssaoBlurShader.setInt("u_SSAOInput", 0);

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
                                          window::aspectRatio, 0.1f, 50.0f)};
    glm::mat4 view{camera.getViewMatrix()};
    glm::mat4 model{glm::mat4(1.0f)};

    geometryShader.use();
    geometryShader.setMat4("u_Projection", projection);
    geometryShader.setMat4("u_View", view);

    // Room cube
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0, 7.0f, 0.0f));
    model = glm::scale(model, glm::vec3(7.5f, 7.5f, 7.5f));
    geometryShader.setMat4("u_Model", model);

    // Invert normals as we're inside the cube
    geometryShader.setBool("u_InvertedNormals", true);
    renderCube();
    geometryShader.setBool("u_InvertedNormals", false);

    // Backpack model on the floor
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
    model = glm::scale(model, glm::vec3(1.0f));

    geometryShader.setMat4("u_Model", model);
    backpackModel.draw(geometryShader);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 2. Generate SSAO texture
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFramebufferId);
    glClear(GL_COLOR_BUFFER_BIT);
    ssaoShader.use();

    // Send kernel + rotation
    for (std::size_t i{0}; i < 64; ++i) {
      ssaoShader.setVec3("u_Samples[" + std::to_string(i) + "]", ssaoKernel[i]);
    }

    ssaoShader.setMat4("u_Projection", projection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPositionTextureId);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormalTextureId);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, noiseTextureId);

    renderQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 3. Blur SSAO texture to remove noise
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFramebufferId);
    glClear(GL_COLOR_BUFFER_BIT);

    ssaoBlurShader.use();
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferId);

    renderQuad();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 4. Lighting Pass
    // Traditional deferred Blinn-Phong lighting with
    // added screen-space ambient occlusion
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    lightingShader.use();

    // Send light relevant uniforms
    glm::vec3 lightPosView{camera.getViewMatrix() * glm::vec4(lightPos, 1.0)};

    lightingShader.setVec3("u_Light.position", lightPosView);
    lightingShader.setVec3("u_Light.color", lightColor);

    // Update attenuation parameters
    constexpr float linear{0.09f};
    constexpr float quadratic{0.032f};

    lightingShader.setFloat("u_Light.linear", linear);
    lightingShader.setFloat("u_Light.quadratic", quadratic);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPositionTextureId);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormalTextureId);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gAlbedoTextureId);
    // Add extra SSAO texture to lighting pass
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlurId);

    renderQuad();

    // glfw: swap buffers and poll IO events
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  geometryShader.remove();
  lightingShader.remove();
  ssaoShader.remove();
  ssaoBlurShader.remove();

  glfwTerminate();
  return 0;
}
