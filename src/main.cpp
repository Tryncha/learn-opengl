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
  layout (location = 1) in vec3 aColor;
  out vec3 ourColor;
  
  void main() {
      gl_Position = vec4(aPos, 1.0f);
      ourColor = aColor;
  }
  )"};

constexpr const char* fragmentShaderSource{R"(
  #version 330 core
  
  in vec3 ourColor;
  out vec4 FragColor;
  
  void main() {
      FragColor = vec4(ourColor, 1.0f);
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
  // each row corresponds to a vertex:
  // 3 floats, 3 floats -> position, color
  const std::array<float, 18> vertices{
     0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,  // top
     0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-right
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f   // bottom-left
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

  // format disabled to clarify syntax
  // clang-format off

  // the function takes the following parameters:
  // glVertexAttribPointer(indexAttr, sizeAttr, dataTypeAttr, needNormalize, stride, pointerWithOffset)

  // position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(0));  // reinterpret_cast<void*>(0) == nullptr
  glEnableVertexAttribArray(0);

  // color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // clang-format on

  // as we only have a single shader, we could also just activate our shader
  // once beforehand if we want to
  glUseProgram(shaderProgram);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

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
