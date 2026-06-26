// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <array>
#include <cmath>
#include <iostream>
#include <string_view>

namespace constants {
constexpr int width{800};
constexpr int height{600};
}  // namespace constants

namespace shaders {
constexpr const char* vertexShaderSource{R"(
  #version 330 core
  
  layout (location = 0) in vec3 aPos;
  
  void main() {
      gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);
  }
  )"};

constexpr const char* fragmentShaderSource{R"(
  #version 330 core
  
  uniform vec4 ourColor;
  out vec4 FragColor;
  
  void main() {
      FragColor = ourColor;
  }
  )"};
}  // namespace shaders

// clang-format off
void framebufferSizeCallback([[maybe_unused]] GLFWwindow* window,
                             int width, int height) {
  glViewport(0, 0, width, height);
}
// clang-format on

void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

void checkForCompileError(GLuint shader, std::string_view message) {
  GLint success{};
  std::array<char, 512> infoLog{};

  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, infoLog.size(), nullptr, infoLog.data());
    std::cerr << message << infoLog.data() << '\n';
  }
}

void checkForLinkError(GLuint program, std::string_view message) {
  GLint success{};
  std::array<char, 512> infoLog{};

  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, infoLog.size(), nullptr, infoLog.data());
    std::cerr << message << infoLog.data() << '\n';
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

  GLFWwindow* window{glfwCreateWindow(constants::width, constants::height,
                                      "LearnOpenGL", nullptr, nullptr)};

  if (!window) {
    std::cout << "Failed to create GLFW window.\n";
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cout << "Failed to initialize GLAD.\n";
    glfwTerminate();
    return -1;
  }

  GLuint vertexShader{glCreateShader(GL_VERTEX_SHADER)};
  glShaderSource(vertexShader, 1, &shaders::vertexShaderSource, nullptr);
  glCompileShader(vertexShader);

  checkForCompileError(vertexShader,
                       "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n");

  GLuint fragmentShader{glCreateShader(GL_FRAGMENT_SHADER)};
  glShaderSource(fragmentShader, 1, &shaders::fragmentShaderSource, nullptr);
  glCompileShader(fragmentShader);

  glCompileShader(fragmentShader);
  checkForCompileError(fragmentShader,
                       "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n");

  GLuint shaderProgram{glCreateProgram()};
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);

  glLinkProgram(shaderProgram);
  checkForLinkError(shaderProgram, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n");

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // clang-format off
  const std::array<float, 9> vertices{
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
  };
  // clang-format on

  GLuint VBO{};
  GLuint VAO{};

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  // we can unbind the VAO afterwards so other VAO calls won't accidentally
  // modify this VAO, but this rarely happens
  glBindVertexArray(0);

  // bind the VAO (it was already bound, but just to demonstrate)
  glBindVertexArray(VAO);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // activate the shader
    glUseProgram(shaderProgram);

    // update the uniform color
    float timeValue{static_cast<float>(glfwGetTime())};
    float greenValue{std::sin(timeValue / 2.0f) + 0.5f};
    int vertexColorLocation{glGetUniformLocation(shaderProgram, "ourColor")};
    glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

    // render the triangle
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);

  glfwTerminate();
  return 0;
}
