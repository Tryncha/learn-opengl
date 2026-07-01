// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <stb_image/stb_image.h>

#include <array>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "shader.h"

// clang-format off
namespace window {
constexpr int width {800};
constexpr int height{600};
}  // namespace constants

namespace camera {
constexpr float baseSpeed  {2.5f};
constexpr float sensitivity{0.1f};

float yaw  {-90.0f};  // rotates y-axis
float pitch{  0.0f};  // rotates x-axis
float roll {  0.0f};  // rotates z-axis

constexpr float pitchUpperLimit{ 89.0f};
constexpr float pitchLowerLimit{-89.0f};

glm::vec3 direction{};
glm::vec3 position {glm::vec3(0.0f, 0.0f,  3.0f)};
glm::vec3 front    {glm::vec3(0.0f, 0.0f, -1.0f)};
glm::vec3 up       {glm::vec3(0.0f, 1.0f,  0.0f)};
}  // namespace camera

namespace timing {
float currentFrame{};
float lastFrame   {0.0f};
// time between current frame and last frame
float deltaTime   {0.0f};
} // namespace framerate

namespace cursor {
bool isFirstInput{true};
float lastX{window::width / 2};
float lastY{window::height / 2};
} // namespace cursor

namespace data {
// each row corresponds to a vertex:
// 3 floats, 2 floats -> position, textureCoords
constexpr std::array<float, (3 + 2) * 6 * 6> vertexData{
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
   0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
  -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

constexpr std::array<glm::vec3, 10> cubePositions{
  glm::vec3( 0.0f,  0.0f,   0.0f), 
  glm::vec3( 2.0f,  5.0f, -15.0f), 
  glm::vec3(-1.5f, -2.2f,  -2.5f),  
  glm::vec3(-3.8f, -2.0f, -12.3f),  
  glm::vec3( 2.4f, -0.4f,  -3.5f),  
  glm::vec3(-1.7f,  3.0f,  -7.5f),  
  glm::vec3( 1.3f, -2.0f,  -2.5f),  
  glm::vec3( 1.5f,  2.0f,  -2.5f), 
  glm::vec3( 1.5f,  0.2f,  -1.5f), 
  glm::vec3(-1.3f,  1.0f,  -1.5f)  
};
} // namespace data
// clang-format on

// GLFW callbacks
void framebufferSizeCallback([[maybe_unused]] GLFWwindow* window, int width,
                             int height) {
  glViewport(0, 0, width, height);
}

// mouse/cursor controls
void cursorPosCallback([[maybe_unused]] GLFWwindow* window, double posX,
                       double posY) {
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

  offsetX *= camera::sensitivity;
  offsetY *= camera::sensitivity;

  camera::yaw += offsetX;
  camera::pitch += offsetY;

  if (camera::pitch > camera::pitchUpperLimit) {
    camera::pitch = camera::pitchUpperLimit;
  }

  if (camera::pitch < camera::pitchLowerLimit) {
    camera::pitch = camera::pitchLowerLimit;
  }

  camera::direction.x = std::cos(glm::radians(camera::yaw)) *
                        std::cos(glm::radians(camera::pitch));

  camera::direction.y = std::sin(glm::radians(camera::pitch));

  camera::direction.z = std::sin(glm::radians(camera::yaw)) *
                        std::cos(glm::radians(camera::pitch));

  camera::front = glm::normalize(camera::direction);
}

// process input
void processKeyboardInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  float cameraSpeed{camera::baseSpeed * timing::deltaTime};

  // WASD controls
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera::position += camera::front * cameraSpeed;
  }

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera::position -=
        glm::normalize(glm::cross(camera::front, camera::up)) * cameraSpeed;
  }

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera::position -= camera::front * cameraSpeed;
  }

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera::position +=
        glm::normalize(glm::cross(camera::front, camera::up)) * cameraSpeed;
  }
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
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSetCursorPosCallback(window, cursorPosCallback);

  // hides the cursor and captures it (makes it stay in the center)
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cout << "Failed to initialize GLAD.\n";
    glfwTerminate();
    return -1;
  }

  // configure global opengl state
  glEnable(GL_DEPTH_TEST);

  Shader ourShader{"src/shaders/vertex.glsl", "src/shaders/fragment.glsl"};

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

  const char* texturePath1{"src/textures/container.jpg"};
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

  const char* texturePath2{"src/textures/awesomeface.png"};
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

  // projection matrix
  const float fovAngle{45.0f};
  const float aspectRatio{static_cast<float>(window::width) /
                          static_cast<float>(window::height)};

  // pass projection matrix to shader, as projection matrix rarely changes
  // there's no need to do this per frame
  glm::mat4 projection{glm::mat4(1.0)};
  projection =
      glm::perspective(glm::radians(fovAngle), aspectRatio, 0.1f, 100.0f);

  ourShader.setMat4("projection", projection);

  while (!glfwWindowShouldClose(window)) {
    // deltaTime calculation to keep consistent the camera speed
    timing::currentFrame = static_cast<float>(glfwGetTime());
    timing::deltaTime = timing::currentFrame - timing::lastFrame;
    timing::lastFrame = timing::currentFrame;

    processKeyboardInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glBindVertexArray(VAO);

    // view matrix
    glm::mat4 view{glm::lookAt(camera::position,
                               camera::position + camera::front, camera::up)};

    ourShader.setMat4("view", view);

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
