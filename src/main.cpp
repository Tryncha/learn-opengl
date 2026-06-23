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

constexpr const char* fragmentShaderSource{R"(
  #version 330 core
  out vec4 FragColor;
  
  void main() {
      FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
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
    return -1;
  }

  // build and compile our shader program
  // vertex shader
  unsigned int vertexShader{};
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &shaders::vertexShaderSource, nullptr);
  glCompileShader(vertexShader);

  // check for shader compile errors
  int success{};
  std::array<char, 512> infoLog{};
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, infoLog.size(), nullptr, infoLog.data());
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog.data() << '\n';
  }

  // fragment shader
  unsigned int fragmentShader{};
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &shaders::fragmentShaderSource, nullptr);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, infoLog.size(), nullptr, infoLog.data());
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
              << infoLog.data() << '\n';
  }

  // link shaders
  unsigned int shaderProgram{};
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, infoLog.size(), nullptr, infoLog.data());
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
              << infoLog.data() << '\n';
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

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

  unsigned int leftVBO{};
  unsigned int leftVAO{};
  glGenVertexArrays(1, &leftVAO);
  glGenBuffers(1, &leftVBO);

  // bind the Vertex Array Object first, then bind and set vertex buffer(s),
  // and then configure vertex attributes(s)
  glBindVertexArray(leftVAO);
  glBindBuffer(GL_ARRAY_BUFFER, leftVBO);
  glBufferData(GL_ARRAY_BUFFER, leftTriangle.size() * sizeof(float),
               leftTriangle.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  // no need to unbind at all as we directly bind a different VAO
  // the next few lines
  // glBindVertexArray(0);

  unsigned int rightVBO{};
  unsigned int rightVAO{};
  glGenVertexArrays(1, &rightVAO);
  glGenBuffers(1, &rightVBO);

  glBindVertexArray(rightVAO);
  glBindBuffer(GL_ARRAY_BUFFER, rightVBO);
  glBufferData(GL_ARRAY_BUFFER, rightTriangle.size() * sizeof(float),
               rightTriangle.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  // render loop
  while (!glfwWindowShouldClose(window)) {
    // input
    processInput(window);

    // render
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw now our rectangle
    glUseProgram(shaderProgram);
    // seeing as we only have a single VAO there's no need to bind it every
    // time, but we'll do so to keep things a bit more organized
    glBindVertexArray(leftVAO);
    // set the count to 6 since we're drawing 6 vertices now, not 3!
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(rightVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // no need to unbind it every time
    // glBindVertexArray(0);

    // glfw: swap buffers and poll IO events
    // (keys pressed/released, mouse moved, etc.)
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // optional: de-allocate all resources once they've outlived their purpose
  glDeleteVertexArrays(1, &leftVAO);
  glDeleteBuffers(1, &leftVBO);
  glDeleteVertexArrays(1, &rightVAO);
  glDeleteBuffers(1, &rightVBO);

  glDeleteProgram(shaderProgram);

  // glfw: terminate, clearing all previously allocated GLFW resources
  glfwTerminate();
  return 0;
}
