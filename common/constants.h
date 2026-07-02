#ifndef CONSTANTS_H
#define CONSTANTS_H

// clang-format off
namespace window {
constexpr int width {800};
constexpr int height{600};
}  // namespace window

namespace constants {
constexpr float minPitch{-89.9f};
constexpr float maxPitch{ 89.9f};
constexpr float minFov  {  1.0f};
constexpr float maxFov  { 45.0f};
}  // namespace constants

namespace timing {
inline float currentFrame{};
inline float lastFrame   {0.0f};
// time between current frame and last frame
inline float deltaTime   {0.0f};
} // namespace timing

namespace cursor {
inline bool isFirstInput{true};
inline float lastX{window::width / 2};
inline float lastY{window::height / 2};
} // namespace cursor
// clang-format on

#endif
