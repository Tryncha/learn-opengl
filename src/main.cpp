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
#include "render.h"
#include "shader.h"
#include "textures.h"

namespace settings {
inline bool isBloomEnable{false};
inline bool isBloomKeyPressed{false};
inline float exposure{1.0f};
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

  if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS &&
      !settings::isBloomKeyPressed) {
    settings::isBloomEnable = !settings::isBloomEnable;
    settings::isBloomKeyPressed = true;
  }

  if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE) {
    settings::isBloomKeyPressed = false;
  }

  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
    if (settings::exposure > 0.0f) {
      settings::exposure -= 0.001f;
    } else {
      settings::exposure = 0.0f;
    }
  } else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    settings::exposure += 0.001f;
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
  Camera camera{glm::vec3(0.0f, 0.0f, 5.0f)};
  glfwSetWindowUserPointer(window, &camera);

  // Build and compile shaders
  Shader baseShader{"shaders/base.vert", "shaders/base.frag"};
  Shader lightShader{"shaders/light.vert", "shaders/light.frag"};
  Shader blurShader{"shaders/blur.vert", "shaders/blur.frag"};
  Shader bloomShader{"shaders/bloom.vert", "shaders/bloom.frag"};

  // Load textures
  // Note that we're loading the textures as an SRGB textures
  unsigned int woodTextureId{loadTexture("assets/textures/wood.png", true)};
  unsigned int containerTextureId{
      loadTexture("assets/textures/container2.png", true)};

  // Configure floating point framebuffer
  unsigned int hdrFramebufferId{};
  glGenFramebuffers(1, &hdrFramebufferId);
  glBindFramebuffer(GL_FRAMEBUFFER, hdrFramebufferId);

  // Create two floating point color buffers
  // One for normal rendering, other for brightness threshold values
  std::array<unsigned int, 2> colorBuffersIds{};
  glGenTextures(2, colorBuffersIds.data());

  for (std::size_t i{0}; i < 2; ++i) {
    glBindTexture(GL_TEXTURE_2D, colorBuffersIds[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window::width, window::height, 0,
                 GL_RGBA, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // We clamp to the edge as the blur filter would
    // otherwise sample repeated texture values!
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Attach texture to framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0 + static_cast<int>(i),
                           GL_TEXTURE_2D, colorBuffersIds[i], 0);
  }

  // Create and attach depth buffer (renderbuffer)
  unsigned int depthRenderbufferId{};
  glGenRenderbuffers(1, &depthRenderbufferId);
  glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbufferId);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window::width,
                        window::height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, depthRenderbufferId);

  // Tell OpenGL which color attachments we'll use
  // (of this framebuffer) for rendering
  std::array<unsigned int, 2> attachments{GL_COLOR_ATTACHMENT0,
                                          GL_COLOR_ATTACHMENT1};
  glDrawBuffers(2, attachments.data());

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "Framebuffer incomplete!\n";
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Ping-pong framebuffer for blurring
  std::array<unsigned int, 2> pingPongFramebuffersIds{};
  std::array<unsigned int, 2> pingPongColorbuffersIds{};

  glGenFramebuffers(2, pingPongFramebuffersIds.data());
  glGenTextures(2, pingPongColorbuffersIds.data());

  for (std::size_t i{0}; i < 2; ++i) {
    glBindFramebuffer(GL_FRAMEBUFFER, pingPongFramebuffersIds[i]);
    glBindTexture(GL_TEXTURE_2D, pingPongColorbuffersIds[i]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window::width, window::height, 0,
                 GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // We clamp to the edge as the blur filter would
    // otherwise sample repeated texture values!
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           pingPongColorbuffersIds[i], 0);

    // Also check if framebuffers are complete (no need for depth buffer)
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      std::cerr << "Framebuffer incomplete!\n";
    }
  }

  // Shaders configuration
  baseShader.use();
  baseShader.setInt("u_DiffuseTexture", 0);

  blurShader.use();
  blurShader.setInt("u_Image", 0);

  bloomShader.use();
  bloomShader.setInt("u_Scene", 0);
  bloomShader.setInt("u_BloomBlur", 1);

  // Light configuration
  // clang-format off
  std::array<glm::vec3, 4> lightPositions{
    glm::vec3( 0.0f, 0.5f,  1.5f),
    glm::vec3(-4.0f, 0.5f, -3.0f),
    glm::vec3( 3.0f, 0.5f,  1.0f),
    glm::vec3(-0.8f, 2.4f, -1.0f)
  };

  std::array<glm::vec3, 4> lightColors{
    glm::vec3( 5.0f, 5.0f,  5.0f),
    glm::vec3(10.0f, 0.0f,  0.0f),
    glm::vec3( 0.0f, 0.0f, 15.0f),
    glm::vec3( 0.0f, 5.0f,  0.0f)
  };
  // clang-format on

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    stabilizeFrame();
    processInput(window);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 1. Render scene into floating point framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFramebufferId);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Configure view/projection matrices
    glm::mat4 projection{glm::perspective(glm::radians(camera.getFov()),
                                          window::aspectRatio, 0.1f, 100.0f)};
    glm::mat4 view{camera.getViewMatrix()};

    // and set uniforms
    baseShader.use();
    baseShader.setMat4("u_Projection", projection);
    baseShader.setMat4("u_View", view);

    for (std::size_t i{0}; i < lightPositions.size(); ++i) {
      baseShader.setVec3("u_Lights[" + std::to_string(i) + "].position",
                         lightPositions[i]);
      baseShader.setVec3("u_Lights[" + std::to_string(i) + "].color",
                         lightColors[i]);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, woodTextureId);

    glm::mat4 model{glm::mat4(1.0f)};
    // Create one large cube that acts as the floor
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0));
    model = glm::scale(model, glm::vec3(12.5f, 0.5f, 12.5f));
    baseShader.setMat4("u_Model", model);
    renderCube();

    // then create multiple cubes as the scenery
    glBindTexture(GL_TEXTURE_2D, containerTextureId);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    baseShader.setMat4("u_Model", model);
    renderCube();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.5f));
    baseShader.setMat4("u_Model", model);
    renderCube();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 2.0));
    model = glm::rotate(model, glm::radians(60.0f),
                        glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    baseShader.setMat4("u_Model", model);
    renderCube();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 2.7f, 4.0));
    model = glm::rotate(model, glm::radians(23.0f),
                        glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(1.25));
    baseShader.setMat4("u_Model", model);
    renderCube();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -3.0));
    model = glm::rotate(model, glm::radians(124.0f),
                        glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    baseShader.setMat4("u_Model", model);
    renderCube();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    baseShader.setMat4("u_Model", model);
    renderCube();

    // Finally show all the light sources as bright cubes
    lightShader.use();
    lightShader.setMat4("u_Projection", projection);
    lightShader.setMat4("u_View", view);

    for (std::size_t i{0}; i < lightPositions.size(); ++i) {
      model = glm::mat4(1.0f);
      model = glm::translate(model, glm::vec3(lightPositions[i]));
      model = glm::scale(model, glm::vec3(0.25f));
      lightShader.setMat4("u_Model", model);
      lightShader.setVec3("u_LightColor", lightColors[i]);
      renderCube();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 2. Blur bright fragments with two-pass Gaussian Blur
    bool doHorizontal{true};
    bool isFirstIt{true};
    std::size_t amount{10};

    blurShader.use();

    for (std::size_t i{0}; i < amount; ++i) {
      glBindFramebuffer(
          GL_FRAMEBUFFER,
          pingPongFramebuffersIds[static_cast<std::size_t>(doHorizontal)]);
      blurShader.setBool("u_DoHorizontal", doHorizontal);

      // Bind texture of other framebuffer (or scene if first iteration)
      glBindTexture(GL_TEXTURE_2D,
                    isFirstIt
                        ? colorBuffersIds[1]
                        : pingPongColorbuffersIds[static_cast<std::size_t>(
                              !doHorizontal)]);
      renderQuad();
      doHorizontal = !doHorizontal;
      if (isFirstIt) isFirstIt = false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 3. Now render floating point color buffer to 2D quad and tonemap HDR
    // colors to default framebuffer's (clamped) color range
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    bloomShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffersIds[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(
        GL_TEXTURE_2D,
        pingPongColorbuffersIds[static_cast<std::size_t>(!doHorizontal)]);

    bloomShader.setBool("u_IsBloomEnable", settings::isBloomEnable);
    bloomShader.setFloat("u_Exposure", settings::exposure);
    renderQuad();

    std::cout << "Bloom: " << (settings::isBloomEnable ? "ON" : "OFF") << '\n';
    std::cout << "Exposure: " << settings::exposure << '\n';

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  baseShader.remove();
  lightShader.remove();
  blurShader.remove();
  bloomShader.remove();

  glfwTerminate();
  return 0;
}
