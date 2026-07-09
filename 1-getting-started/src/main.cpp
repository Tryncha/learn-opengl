// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <stb_image/stb_image.h>

#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <string>

#include "camera.h"
#include "constants.h"
#include "data.h"
#include "shader.h"

// clang-format off
namespace window {
constexpr int width {800};
constexpr int height{600};
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

// mouse/cursor controls
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

// offsetY means how much we scrolled vertically
void scrollCallback([[maybe_unused]] GLFWwindow* window,
                    [[maybe_unused]] double offsetX, double offsetY) {
  auto& camera{*static_cast<Camera*>(glfwGetWindowUserPointer(window))};
  camera.processScrollInput(offsetY);
}

// process input
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

  GLFWwindow* window{glfwCreateWindow(window::width, window::height,
                                      "LearnOpenGL", nullptr, nullptr)};

  if (!window) {
    std::cout << "Failed to create GLFW window.\n";
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);

  Camera camera{glm::vec3(0.0f, 0.0f, 3.0f)};

  // save camera as "user pointer" to retrieve later by reference
  glfwSetWindowUserPointer(window, &camera);

  // callbacks
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSetCursorPosCallback(window, cursorPosCallback);
  glfwSetScrollCallback(window, scrollCallback);

  // hides the cursor and captures it (makes it stay in the center)
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cout << "Failed to initialize GLAD.\n";
    glfwTerminate();
    return -1;
  }

  // configure global opengl state
  glEnable(GL_DEPTH_TEST);

  Shader ourShader{
      (std::string(CHAPTER_DIR) + "/shaders/vertex.glsl").c_str(),
      (std::string(CHAPTER_DIR) + "/shaders/fragment.glsl").c_str()};

  GLuint VBO{};
  GLuint VAO{};

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, data::vertexData.size() * sizeof(float),
               data::vertexData.data(), GL_STATIC_DRAW);

  // sizeof(float) == 4 bytes

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        reinterpret_cast<void*>(0));
  glEnableVertexAttribArray(0);

  // textureCoords attribute
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  int width{};
  int height{};
  int nrChannels{};

  // load and create a texture 1
  GLuint texture1{};

  glGenTextures(1, &texture1);
  glBindTexture(GL_TEXTURE_2D, texture1);

  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // tell stb_image.h to flip loaded texture's on the y-axis.
  stbi_set_flip_vertically_on_load(true);

  const char* texturePath1{"resources/container.jpg"};
  // load image, create texture and generate mipmaps
  unsigned char* textureData1{
      stbi_load(texturePath1, &width, &height, &nrChannels, 0)};

  if (textureData1) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, textureData1);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cerr << "Failed to load texture 1.\n";
  }

  stbi_image_free(textureData1);

  // load and create a texture 2
  GLuint texture2{};

  glGenTextures(1, &texture2);
  glBindTexture(GL_TEXTURE_2D, texture2);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  const char* texturePath2{"resources/awesomeface.png"};
  unsigned char* textureData2{
      stbi_load(texturePath2, &width, &height, &nrChannels, 0)};

  if (textureData2) {
    // note that the awesomeface.png has transparency and thus an alpha channel,
    // so make sure to tell OpenGL the data type is of GL_RGBA
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, textureData2);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cerr << "Failed to load texture 2.\n";
  }

  stbi_image_free(textureData2);

  ourShader.use();
  ourShader.setInt("ourTexture1", 0);
  ourShader.setInt("ourTexture2", 1);

  while (!glfwWindowShouldClose(window)) {
    // deltaTime calculation to keep consistent the camera speed
    timing::currentFrame = static_cast<float>(glfwGetTime());
    timing::deltaTime = timing::currentFrame - timing::lastFrame;
    timing::lastFrame = timing::currentFrame;

    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    // activate shader
    ourShader.use();

    // projection matrix, now need to change every frame
    const float aspectRatio{static_cast<float>(window::width) /
                            static_cast<float>(window::height)};

    glm::mat4 projection{glm::mat4(1.0)};
    projection = glm::perspective(glm::radians(camera.getFov()), aspectRatio,
                                  0.1f, 100.0f);

    ourShader.setMat4("projection", projection);

    // view matrix
    ourShader.setMat4("view", camera.getViewMatrix());

    glBindVertexArray(VAO);

    // model matrix
    for (std::size_t i{0}; i < data::cubePositions.size(); ++i) {
      float angle{20.0f * static_cast<float>(i)};

      glm::mat4 model{glm::mat4(1.0)};
      model = glm::translate(model, data::cubePositions[i]);
      model =
          glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

      ourShader.setMat4("model", model);

      // render container
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  ourShader.remove();

  glfwTerminate();
  return 0;
}
