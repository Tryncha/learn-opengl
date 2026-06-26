#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Shader {
 public:
  Shader(const char* vertexPath, const char* fragmentPath);

  // use/activate the shader and delete it
  void use();
  void remove();

  // utility uniform functions
  void setInt(const std::string& name, int value) const;
  void setFloat(const std::string& name, float value) const;
  void setBool(const std::string& name, bool value) const;

 private:
  GLuint shaderProgram;
};

#endif
