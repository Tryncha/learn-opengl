#include "camera.h"

// constructor with vectors
Camera::Camera(const glm::vec3& position, const glm::vec3& up, float yaw,
               float pitch)
    : m_position(position),
      m_up(up),
      m_front(DEFAULT_FRONT),
      m_worldUp(DEFAULT_UP),
      m_yaw(yaw),
      m_pitch(pitch),
      m_baseSpeed(DEFAULT_BASE_SPEED),
      m_sensitivity(DEFAULT_SENSITIVITY),
      m_fov(DEFAULT_FOV) {
  updateVectors();
}

// constructor with scalar values
Camera::Camera(float posX, float posY, float posZ, float upX, float upY,
               float upZ, float yaw, float pitch)
    : m_position(glm::vec3(posX, posY, posZ)),
      m_up(glm::vec3(upX, upY, upZ)),
      m_front(DEFAULT_FRONT),
      m_worldUp(DEFAULT_UP),
      m_yaw(yaw),
      m_pitch(pitch),
      m_baseSpeed(DEFAULT_BASE_SPEED),
      m_sensitivity(DEFAULT_SENSITIVITY),
      m_fov(DEFAULT_FOV) {
  updateVectors();
}

// getters/setters
float Camera::getFov() const { return m_fov; }

glm::mat4 Camera::getViewMatrix() const {
  // manual process to create lookAt matrix
  glm::vec3 axisZ{glm::normalize(m_position - (m_position + m_front))};
  glm::vec3 axisX{glm::normalize(glm::cross(glm::normalize(m_up), axisZ))};
  glm::vec3 axisY{glm::cross(axisZ, axisX)};

  // glm::mat4 receives the columns of the matrix, not the rows
  glm::mat4 myLookAt{
      glm::mat4(glm::vec4(axisX.x, axisY.x, axisZ.x, 0.0f),
                glm::vec4(axisX.y, axisY.y, axisZ.y, 0.0f),
                glm::vec4(axisX.z, axisY.z, axisZ.z, 0.0f),
                glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)) *
      glm::mat4(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f),
                glm::vec4(0.0f, 1.0f, 0.0f, 0.0f),
                glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
                glm::vec4(-m_position.x, -m_position.y, -m_position.z, 1.0f))};

  return myLookAt;
  // return glm::lookAt(m_position, m_position + m_front, m_up);
}

// member functions
// calculates the front vector from the camera's euler angles
// clang-format off
void Camera::updateVectors() {
  glm::vec3 newFront{};

  newFront.x = std::cos(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
  newFront.y = std::sin(glm::radians(m_pitch));
  newFront.z = std::sin(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
  
  m_front = glm::normalize(newFront);
  m_right = glm::normalize(glm::cross(m_front, m_worldUp));
  m_up    = glm::normalize(glm::cross(m_right, m_front));
}
// clang-format on

void Camera::processKeyboardInput(GLFWwindow* window) {
  float cameraSpeed{m_baseSpeed * timing::deltaTime};

  // WASD controls
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    m_position += m_front * cameraSpeed;

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    m_position -= m_right * cameraSpeed;

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    m_position -= m_front * cameraSpeed;

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    m_position += m_right * cameraSpeed;

  // this one-line keeps the user at the ground level (xz plane)
  m_position.y = 0.0f;
}

void Camera::processMouseInput(double offsetX, double offsetY,
                               bool hasConstrainPitch) {
  offsetX *= m_sensitivity;
  offsetY *= m_sensitivity;

  m_yaw += static_cast<float>(offsetX);
  m_pitch += static_cast<float>(offsetY);

  if (hasConstrainPitch) {
    if (m_pitch < constants::minPitch) m_pitch = constants::minPitch;
    if (m_pitch > constants::maxPitch) m_pitch = constants::maxPitch;
  }

  updateVectors();
}

void Camera::processScrollInput(double offsetY) {
  m_fov -= static_cast<float>(offsetY);

  if (m_fov < constants::minFov) m_fov = constants::minFov;
  if (m_fov > constants::maxFov) m_fov = constants::maxFov;
}
