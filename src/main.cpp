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
#include "render.h"
#include "shader.h"
#include "textures.h"

// Light configuration
namespace light {
inline glm::vec3 position{glm::vec3(-2.0f, 4.0f, -1.0f)};
}  // namespace light

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
  Camera camera{glm::vec3(0.0f, 0.0f, 3.0f)};
  glfwSetWindowUserPointer(window, &camera);

  // Build and compile shaders
  Shader ourShader{"shaders/vert.glsl", "shaders/frag.glsl"};
  Shader depthShader{"shaders/vert_depth.glsl", "shaders/frag_depth.glsl"};
  Shader debugDepthShader{"shaders/vert_debug_depth.glsl",
                          "shaders/frag_debug_depth.glsl"};

  // Setup VAO (and VBO)
  unsigned int planeVBO{};

  glGenVertexArrays(1, &meshes::planeVAO);
  glGenBuffers(1, &planeVBO);

  glBindVertexArray(meshes::planeVAO);
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
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                        reinterpret_cast<void*>(6 * sizeof(float)));

  glBindVertexArray(0);

  // Load textures
  unsigned int woodTex{loadTexture("assets/textures/wood.png", false)};

  // Configure depth map framebuffer
  constexpr int shadowWidth{1024};
  constexpr int shadowHeight{1024};

  unsigned int depthMapFBO{};
  glGenFramebuffers(1, &depthMapFBO);

  // Create depth texture
  unsigned int depthMap{};
  glGenTextures(1, &depthMap);

  glBindTexture(GL_TEXTURE_2D, depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight,
               0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  constexpr std::array<float, 4> borderColor{1.0f, 1.0f, 1.0f, 1.0f};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor.data());

  // Attach depth texture as FBO's depth buffer
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         depthMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Shaders configuration
  ourShader.use();
  ourShader.setInt("u_DiffuseTexture", 0);
  ourShader.setInt("u_ShadowMap", 1);

  debugDepthShader.use();
  debugDepthShader.setInt("u_DepthMap", 0);

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    stabilizeFrame();
    processInput(window);

    // Change light position over time
    light::position.x = std::sin(static_cast<float>(glfwGetTime())) * 3.0f;
    light::position.z = std::cos(static_cast<float>(glfwGetTime())) * 2.0f;
    light::position.y =
        5.0f + std::cos(static_cast<float>(glfwGetTime())) * 1.0f;

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 1. Render depth of the scene to texture (from light's perspective)
    glCullFace(GL_FRONT);
    glm::mat4 lightProjection{glm::mat4(1.0f)};
    glm::mat4 lightView{glm::mat4(1.0f)};
    glm::mat4 lightSpace{glm::mat4(1.0f)};

    constexpr float nearPlane{1.0f};
    constexpr float farPlane{7.5f};

    lightProjection =
        glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
    lightView = glm::lookAt(light::position, glm::vec3(0.0f),
                            glm::vec3(0.0f, 1.0f, 0.0f));
    lightSpace = lightProjection * lightView;

    // Render scene from light's point of view
    depthShader.use();
    depthShader.setMat4("u_LightSpace", lightSpace);

    glViewport(0, 0, shadowWidth, shadowHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

    glClear(GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, woodTex);
    renderScene(depthShader);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Reset viewport
    glViewport(0, 0, window::width, window::height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);

    // 2. Render scene as normal using the generated depth/shadow map
    ourShader.use();

    glm::mat4 projection{glm::perspective(glm::radians(camera.getFov()),
                                          window::aspectRatio, 0.1f, 100.0f)};
    glm::mat4 view{camera.getViewMatrix()};

    // Set transformation matrices uniforms
    ourShader.setMat4("u_Projection", projection);
    ourShader.setMat4("u_View", view);

    // Set light uniforms
    ourShader.setVec3("u_ViewPos", camera.getPosition());
    ourShader.setVec3("u_LightPos", light::position);
    ourShader.setMat4("u_LightSpace", lightSpace);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, woodTex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    renderScene(ourShader);

    // Optional. Render Depth map to quad for visual debugging
    // debugDepthShader.use();
    // debugDepthShader.setFloat("u_NearPlane", nearPlane);
    // debugDepthShader.setFloat("u_FarPlane", farPlane);

    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, depthMap);

    // renderQuad();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  depthShader.remove();
  debugDepthShader.remove();
  glfwTerminate();
  return 0;
}
