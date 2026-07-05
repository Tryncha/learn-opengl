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

glm::vec3 Camera::getPosition() const { return m_position; }
glm::vec3 Camera::getFront() const { return m_front; }
glm::mat4 Camera::getViewMatrix() const {
  return glm::lookAt(m_position, m_position + m_front, m_up);
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

// Minecraft-like camera controls
void Camera::processKeyboardInput(GLFWwindow* window) {
  float cameraSpeed{m_baseSpeed * timing::deltaTime};

  // WASD controls
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    // to keep user on xz plane
    m_position += glm::vec3(m_front.x, 0.0f, m_front.z) * cameraSpeed;

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    m_position -= m_right * cameraSpeed;

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    // to keep user on xz plane
    m_position -= glm::vec3(m_front.x, 0.0f, m_front.z) * cameraSpeed;

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    m_position += m_right * cameraSpeed;

  // go up
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    m_position += m_worldUp * cameraSpeed;

  // go down
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    m_position -= m_worldUp * cameraSpeed;
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
