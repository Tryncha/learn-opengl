#include "shader.h"

void checkCompileError(GLuint shader, std::string_view message) {
  GLint success{};
  std::array<char, 1024> infoLog{};

  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, infoLog.size(), nullptr, infoLog.data());
    std::cerr << message << '\n' << infoLog.data() << '\n';
  }
}

void checkLinkError(GLuint program, std::string_view message) {
  GLint success{};
  std::array<char, 1024> infoLog{};

  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, infoLog.size(), nullptr, infoLog.data());
    std::cerr << message << '\n' << infoLog.data() << '\n';
  }
}

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
  // 1. retrieve the vertex/fragment source code from filePath
  std::string vShaderRawCode{};
  std::string fShaderRawCode{};
  std::ifstream vShaderFile{};
  std::ifstream fShaderFile{};

  // ensure ifstream objects can throw exceptions:
  vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    // open files
    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);
    std::stringstream vShaderStream{};
    std::stringstream fShaderStream{};

    // read file's buffer contents into streams
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();

    // close file handlers
    vShaderFile.close();
    fShaderFile.close();

    // convert stream into string
    vShaderRawCode = vShaderStream.str();
    fShaderRawCode = fShaderStream.str();
  } catch (std::ifstream::failure& e) {
    std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what()
              << '\n';
  }

  const char* vShaderCode = vShaderRawCode.c_str();
  const char* fShaderCode = fShaderRawCode.c_str();

  // 2. compile shaders
  GLuint vShader{glCreateShader(GL_VERTEX_SHADER)};
  glShaderSource(vShader, 1, &vShaderCode, nullptr);
  glCompileShader(vShader);
  checkCompileError(vShader, "ERROR::SHADER::VERTEX::COMPILATION_FAILED");

  GLuint fShader{glCreateShader(GL_FRAGMENT_SHADER)};
  glShaderSource(fShader, 1, &fShaderCode, nullptr);
  glCompileShader(fShader);
  checkCompileError(fShader, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED");

  // shader program
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vShader);
  glAttachShader(shaderProgram, fShader);

  glLinkProgram(shaderProgram);
  checkLinkError(shaderProgram, "ERROR::SHADER::PROGRAM::LINKING_FAILED");

  // delete the shaders as they're linked into our program now
  // and no longer necessary
  glDeleteShader(vShader);
  glDeleteShader(fShader);
}

// use/activate the shader and delete it
void Shader::use() { glUseProgram(shaderProgram); }
void Shader::remove() { glDeleteProgram(shaderProgram); }

// utility uniform functions
void Shader::setInt(const std::string& name, int value) const {
  glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
  glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

void Shader::setBool(const std::string& name, bool value) const {
  setInt(name, static_cast<int>(value));
}

// utility uniform vector functions
void Shader::setVec2(const std::string& name, const glm::vec2& vec) const {
  glUniform2fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &vec[0]);
}
void Shader::setVec2(const std::string& name, float x, float y) const {
  glUniform2f(glGetUniformLocation(shaderProgram, name.c_str()), x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& vec) const {
  glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &vec[0]);
}
void Shader::setVec3(const std::string& name, float x, float y, float z) const {
  glUniform3f(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& vec) const {
  glUniform4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &vec[0]);
}
void Shader::setVec4(const std::string& name, float x, float y, float z,
                     float w) const {
  glUniform4f(glGetUniformLocation(shaderProgram, name.c_str()), x, y, z, w);
}

// utility uniform matrix functions
void Shader::setMat2(const std::string& name, const glm::mat2& mat) const {
  glUniformMatrix2fv(glGetUniformLocation(shaderProgram, name.c_str()), 1,
                     GL_FALSE, &mat[0][0]);
}
void Shader::setMat3(const std::string& name, const glm::mat3& mat) const {
  glUniformMatrix3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1,
                     GL_FALSE, &mat[0][0]);
}
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1,
                     GL_FALSE, &mat[0][0]);
}
