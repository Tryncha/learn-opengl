// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <array>
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
  // glfw: initialize
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW.\n";
    return -1;
  }

  // and configure
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

  // fragment shader
  GLuint fragmentShader{glCreateShader(GL_FRAGMENT_SHADER)};
  glShaderSource(fragmentShader, 1, &shaders::fragmentShaderSource, nullptr);
  glCompileShader(fragmentShader);

  glCompileShader(fragmentShader);
  checkForCompileError(fragmentShader,
                       "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n");

  // link shaders
  GLuint shaderProgram{glCreateProgram()};
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);

  glLinkProgram(shaderProgram);
  checkForLinkError(shaderProgram, "ERROR::SHADER::PROGRAM::LINKING_FAILED\n");

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // clang-format off
  // const std::array<float, 9> triangleVerts{
  //   -0.5f, -0.5f, 0.0f,
  //    0.5f, -0.5f, 0.0f,
  //    0.0f,  0.5f, 0.0f
  // };

  const std::array<float, 12> rectangleVerts{
     0.5f,  0.5f, 0.0f,  // 0, top-right
     0.5f, -0.5f, 0.0f,  // 1, bottom-right
    -0.5f, -0.5f, 0.0f,  // 2, bottom-left
    -0.5f,  0.5f, 0.0f   // 3, top-left
  };

  const std::array<int, 6> rectangleIdxs{
    0, 1, 3,  // first triangle
    1, 2, 3   // second triangle
  };
  // clang-format on

  GLuint VBO{};
  GLuint VAO{};
  GLuint EBO{};

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  // bind the Vertex Array Object first, then bind and set vertex buffer(s),
  // and then configure vertex attributes(s)
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, rectangleVerts.size() * sizeof(float),
               rectangleVerts.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               rectangleIdxs.size() * sizeof(unsigned int),
               rectangleIdxs.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  // note that this is allowed, the call to glVertexAttribPointer registered VBO
  // as the vertex attribute's bound vertex buffer object so afterwards we can
  // safely unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // remember: do NOT unbind the EBO while a VAO is active as the bound element
  // buffer object IS stored in the VAO; keep the EBO bound.
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // you can unbind the VAO afterwards so other VAO calls won't accidentally
  // modify this VAO, but this rarely happens.
  // modifying other VAOs requires a call to glBindVertexArray anyways so we
  // generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
  glBindVertexArray(0);

  // uncomment this call to draw in wireframe polygons
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // render loop
  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    // render
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw now our rectangle
    glUseProgram(shaderProgram);
    // seeing as we only have a single VAO there's no need to bind it every
    // time, but we'll do so to keep things a bit more organized
    glBindVertexArray(VAO);
    // glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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
