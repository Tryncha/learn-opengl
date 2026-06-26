// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <array>
#include <iostream>

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

constexpr const char* orangeFragmentShaderSource{R"(
  #version 330 core
  out vec4 FragColor;
  
  void main() {
      FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
  }
  )"};

constexpr const char* yellowFragmentShaderSource{R"(
  #version 330 core
  out vec4 FragColor;
  
  void main() {
      FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);
  }
  )"};
}  // namespace shaders

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// clang-format off
void framebufferSizeCallback([[maybe_unused]] GLFWwindow* window,
                             int width, int height) {
  glViewport(0, 0, width, height);
}
// clang-format on

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
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
    std::cout << message << infoLog.data() << '\n';
  }
}

void checkForLinkError(GLuint program, std::string_view message) {
  GLint success{};
  std::array<char, 512> infoLog{};

  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, infoLog.size(), nullptr, infoLog.data());
    std::cout << message << infoLog.data() << '\n';
  }
}

int main(int, char**) {
  // glfw: initialize and configure
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // glfw window creation
  GLFWwindow* window{glfwCreateWindow(constants::width, constants::height,
                                      "LearnOpenGL", nullptr, nullptr)};

  if (!window) {
    std::cout << "Failed to create GLFW window.\n";
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  // glad: load all OpenGL function pointers
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cout << "Failed to initialize GLAD.\n";
    glfwTerminate();
    return -1;
  }

  // build and compile our shader program
  GLuint vertexShader{glCreateShader(GL_VERTEX_SHADER)};
  glShaderSource(vertexShader, 1, &shaders::vertexShaderSource, nullptr);
  glCompileShader(vertexShader);

  // check for shader compile errors
  checkForCompileError(vertexShader,
                       "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n");

  // orange fragment shader
  GLuint orangeFragmentShader{glCreateShader(GL_FRAGMENT_SHADER)};
  glShaderSource(orangeFragmentShader, 1, &shaders::orangeFragmentShaderSource,
                 nullptr);

  glCompileShader(orangeFragmentShader);
  checkForCompileError(orangeFragmentShader,
                       "ERROR::ORANGE::SHADER::FRAGMENT::COMPILATION_FAILED\n");

  // yellow fragment shader
  GLuint yellowFragmentShader{glCreateShader(GL_FRAGMENT_SHADER)};
  glShaderSource(yellowFragmentShader, 1, &shaders::yellowFragmentShaderSource,
                 nullptr);

  glCompileShader(yellowFragmentShader);
  checkForCompileError(yellowFragmentShader,
                       "ERROR::YELLOW::SHADER::FRAGMENT::COMPILATION_FAILED\n");

  // link the first program object
  GLuint orangeShaderProgram{glCreateProgram()};
  glAttachShader(orangeShaderProgram, vertexShader);
  glAttachShader(orangeShaderProgram, orangeFragmentShader);

  glLinkProgram(orangeShaderProgram);
  checkForLinkError(orangeShaderProgram,
                    "ERROR::ORANGE::SHADER::PROGRAM::LINKING_FAILED\n");

  // then link the second program object using a different fragment shader
  // (but same vertex shader)
  GLuint yellowShaderProgram{glCreateProgram()};
  glAttachShader(yellowShaderProgram, vertexShader);
  glAttachShader(yellowShaderProgram, yellowFragmentShader);

  glLinkProgram(yellowShaderProgram);
  checkForLinkError(yellowShaderProgram,
                    "ERROR::YELLOW::SHADER::PROGRAM::LINKING_FAILED\n");

  glDeleteShader(vertexShader);
  glDeleteShader(orangeFragmentShader);
  glDeleteShader(yellowFragmentShader);

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // clang-format off
  const std::array<float, 9> leftTriangle{
    -0.5f,  -0.5f, 0.0f,
    -0.05f, -0.5f, 0.0f,
    -0.05f,  0.5f, 0.0f,
  };

  const std::array<float, 9> rightTriangle{
    0.5f,  -0.5f, 0.0f,
    0.05f, -0.5f, 0.0f,
    0.05f,  0.5f, 0.0f
  };
  // clang-format on

  std::array<GLuint, 2> vbos{};
  std::array<GLuint, 2> vaos{};

  // we can also generate multiple VBOs or VAOs at the same time
  glGenBuffers(2, vbos.data());
  glGenVertexArrays(2, vaos.data());

  // first triangle setup
  glBindVertexArray(vaos[0]);
  glBindBuffer(GL_ARRAY_BUFFER, vbos[0]);
  glBufferData(GL_ARRAY_BUFFER, leftTriangle.size() * sizeof(float),
               leftTriangle.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  // second triangle setup
  glBindVertexArray(vaos[1]);
  glBindBuffer(GL_ARRAY_BUFFER, vbos[1]);
  glBufferData(GL_ARRAY_BUFFER, rightTriangle.size() * sizeof(float),
               rightTriangle.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  // render loop
  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    // render
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // now when we draw the triangle we first use the vertex and orange fragment
    // shader from the first program
    glUseProgram(orangeShaderProgram);
    glBindVertexArray(vaos[0]);
    // this call should output an orange triangle
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glUseProgram(yellowShaderProgram);
    glBindVertexArray(vaos[1]);
    // this call should output a yellow triangle
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // glfw: swap buffers and poll IO events
    // (keys pressed/released, mouse moved, etc.)
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // optional: de-allocate all resources once they've outlived their purpose
  glDeleteBuffers(2, vbos.data());
  glDeleteVertexArrays(2, vaos.data());
  glDeleteProgram(orangeShaderProgram);
  glDeleteProgram(yellowShaderProgram);

  // glfw: terminate, clearing all previously allocated GLFW resources
  glfwTerminate();
  return 0;
}
