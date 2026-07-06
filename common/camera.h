#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "constants.h"

class Camera {
 public:
  // constructor with vectors
  Camera(const glm::vec3& position = s_defaultPosition,
         const glm::vec3& up = s_defaultUp, float yaw = s_defaultYaw,
         float pitch = s_defaultPitch);

  // constructor with scalar values
  Camera(float posX, float posY, float posZ, float upX, float upY, float upZ,
         float yaw = s_defaultYaw, float pitch = s_defaultPitch);

  // getters/setters
  float getFov() const;
  glm::vec3 getPosition() const;
  glm::vec3 getFront() const;
  glm::mat4 getViewMatrix() const;

  // member functions
  // calculates the front vector from the camera's euler angles
  void updateVectors();
  void processKeyboardInput(GLFWwindow* window);
  void processMouseInput(double offsetX, double offsetY,
                         bool hasConstrainPitch = true);
  void processScrollInput(double offsetY);

 private:
  // clang-format off
  // defaults
  static constexpr glm::vec3 s_defaultPosition{glm::vec3(0.0f, 0.0f,  0.0f)};
  static constexpr glm::vec3 s_defaultUp      {glm::vec3(0.0f, 1.0f,  0.0f)};
  static constexpr glm::vec3 s_defaultFront   {glm::vec3(0.0f, 0.0f, -1.0f)};
  
  // euler angles
  static constexpr float s_defaultYaw  {-90.0f};
  static constexpr float s_defaultPitch{  0.0f};
  
  // camera options
  static constexpr float s_defaultBaseSpeed  { 2.5f};
  static constexpr float s_defaultSensitivity{ 0.1f};
  static constexpr float s_defaultFov        {45.0f};
  // clang-format on

  // class members
  glm::vec3 m_position{};
  glm::vec3 m_up{};
  glm::vec3 m_front{};

  glm::vec3 m_direction{};
  glm::vec3 m_right{};
  glm::vec3 m_worldUp{};

  // euler angles
  // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in
  // a direction vector pointing to the right so we initially rotate a
  // bit to the left
  float m_yaw{};    // rotates y-axis
  float m_pitch{};  // rotates x-axis

  // camera options
  float m_baseSpeed{};
  float m_sensitivity{};
  float m_fov{};
};

#endif
