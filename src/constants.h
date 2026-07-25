#ifndef CONSTANTS_H
#define CONSTANTS_H

// clang-format off
namespace constants {
constexpr float minPitch{-89.9f};
constexpr float maxPitch{ 89.9f};
constexpr float minFov  {  1.0f};
constexpr float maxFov  { 45.0f};
}  // namespace constants

namespace window {
constexpr int width {1280};
constexpr int height{720};
constexpr float aspectRatio{static_cast<float>(width) /
                            static_cast<float>(height)};
}  // namespace window

namespace cursor {
inline bool isFirstInput{true};
inline float lastX{window::width / 2};
inline float lastY{window::height / 2};
}  // namespace cursor

namespace timing {
inline float currentFrame{};
inline float lastFrame   {0.0f};
// time between current frame and last frame
inline float deltaTime   {0.0f};
} // namespace timing

namespace camera {
constexpr float nearPlane{0.1f};
constexpr float farPlane{100.0f};
}  // namespace camera
// clang-format on

#endif
