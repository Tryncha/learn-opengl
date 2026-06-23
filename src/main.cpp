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
  const std::array<float, 2 * 9> vertices{
    // left triangle
    -0.5f,  -0.5f, 0.0f,
    -0.05f, -0.5f, 0.0f,
    -0.05f,  0.5f, 0.0f,

    // right triangle
     0.5f,  -0.5f, 0.0f,
     0.05f, -0.5f, 0.0f,
     0.05f,  0.5f, 0.0f
  };
  // clang-format on

  unsigned int VBO{};
  unsigned int VAO{};

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  // bind the Vertex Array Object first, then bind and set vertex buffer(s),
  // and then configure vertex attributes(s)
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               vertices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  // note that this is allowed, the call to glVertexAttribPointer registered VBO
  // as the vertex attribute's bound vertex buffer object so afterwards we can
  // safely unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // you can unbind the VAO afterwards so other VAO calls won't accidentally
  // modify this VAO, but this rarely happens.
  // modifying other VAOs requires a call to glBindVertexArray anyways so we
  // generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
  glBindVertexArray(0);

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
    glBindVertexArray(VAO);
    // set the count to 6 since we're drawing 6 vertices now, not 3!
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // no need to unbind it every time
    // glBindVertexArray(0);

    // glfw: swap buffers and poll IO events
    // (keys pressed/released, mouse moved, etc.)
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // optional: de-allocate all resources once they've outlived their purpose
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);

  // glfw: terminate, clearing all previously allocated GLFW resources
  glfwTerminate();
  return 0;
}
