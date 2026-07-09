#ifndef SHADER_H
#define SHADER_H

#include <glm/glm.hpp>
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

  // utility uniform vector functions
  void setVec2(const std::string& name, const glm::vec2& vec) const;
  void setVec2(const std::string& name, float x, float y) const;

  void setVec3(const std::string& name, const glm::vec3& vec) const;
  void setVec3(const std::string& name, float x, float y, float z) const;

  void setVec4(const std::string& name, const glm::vec4& vec) const;
  void setVec4(const std::string& name, float x, float y, float z,
               float w) const;

  // utility uniform matrix functions
  void setMat2(const std::string& name, const glm::mat2& mat) const;
  void setMat3(const std::string& name, const glm::mat3& mat) const;
  void setMat4(const std::string& name, const glm::mat4& mat) const;

 private:
  unsigned int m_shaderProgram;
};

#endif
