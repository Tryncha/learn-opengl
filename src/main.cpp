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
inline bool isHdrEnable{false};
inline bool isHdrKeyPressed{false};
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

  if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS &&
      !settings::isHdrKeyPressed) {
    settings::isHdrEnable = !settings::isHdrEnable;
    settings::isHdrKeyPressed = true;
  }

  if (glfwGetKey(window, GLFW_KEY_H) == GLFW_RELEASE) {
    settings::isHdrKeyPressed = false;
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
  Shader ourShader{"shaders/vert.glsl", "shaders/frag.glsl"};
  Shader hdrShader{"shaders/vert_hdr.glsl", "shaders/frag_hdr.glsl"};

  // Load textures
  // Note that we're loading the texture as an SRGB texture
  unsigned int woodTextureId{loadTexture("assets/textures/wood.png", true)};

  // Configure floating point framebuffer
  unsigned int hdrFramebufferId{};
  glGenFramebuffers(1, &hdrFramebufferId);

  // Create floating point color buffer
  unsigned int colorBufferId{};
  glGenTextures(1, &colorBufferId);

  glBindTexture(GL_TEXTURE_2D, colorBufferId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window::width, window::height, 0,
               GL_RGBA, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Create depth buffer (renderbuffer)
  unsigned int depthRenderbufferId{};
  glGenRenderbuffers(1, &depthRenderbufferId);
  glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbufferId);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window::width,
                        window::height);

  // Attach buffers
  glBindFramebuffer(GL_FRAMEBUFFER, hdrFramebufferId);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         colorBufferId, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, depthRenderbufferId);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cerr << "Framebuffer incomplete!\n";
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Shaders configuration
  ourShader.use();
  ourShader.setInt("u_DiffuseTexture", 0);
  hdrShader.use();
  hdrShader.setInt("u_HdrBuffer", 0);

  // Light configuration
  // clang-format off
  std::array<glm::vec3, 4> lightPositions{
    glm::vec3( 0.0f,  0.0f, 49.5f),
    glm::vec3(-1.4f, -1.9f,  9.0f),
    glm::vec3( 0.0f, -1.8f,  4.0f),
    glm::vec3( 0.8f, -1.7f,  6.0f)
  };

  std::array<glm::vec3, 4> lightColors{
    glm::vec3(200.0f, 200.0f, 200.0f),
    glm::vec3(  0.1f,   0.0f,   0.0f),
    glm::vec3(  0.0f,   0.0f,   0.2f),
    glm::vec3(  0.0f,   0.1f,   0.0f)
  };
  // clang-format on

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    stabilizeFrame();
    processInput(window);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 1. Render scene into floating point framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFramebufferId);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Configure view/projection matrices
    glm::mat4 projection{glm::perspective(glm::radians(camera.getFov()),
                                          window::aspectRatio, 0.1f, 100.0f)};
    glm::mat4 view{camera.getViewMatrix()};

    // and set uniforms
    ourShader.use();
    ourShader.setMat4("u_Projection", projection);
    ourShader.setMat4("u_View", view);

    for (std::size_t i{0}; i < lightPositions.size(); ++i) {
      ourShader.setVec3("u_Lights[" + std::to_string(i) + "].position",
                        lightPositions[i]);
      ourShader.setVec3("u_Lights[" + std::to_string(i) + "].color",
                        lightColors[i]);
    }

    // Render tunnel
    glm::mat4 model{glm::mat4(1.0f)};
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0));
    model = glm::scale(model, glm::vec3(2.5f, 2.5f, 27.5f));

    ourShader.setMat4("u_Model", model);
    ourShader.setBool("u_InverseNormals", true);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, woodTextureId);

    renderCube();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 2. Now render floating point color buffer to 2D quad and tonemap HDR
    // colors to default framebuffer's (clamped) color range
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    hdrShader.use();
    hdrShader.setInt("u_IsHdrEnable", settings::isHdrEnable);
    hdrShader.setFloat("u_Exposure", settings::exposure);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBufferId);
    renderQuad();

    // Check HDR status
    std::cout << "HDR: " << (settings::isHdrEnable ? "ON" : "OFF") << '\n';
    std::cout << "Exposure: " << settings::exposure << '\n';

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  ourShader.remove();

  glfwTerminate();
  return 0;
}
