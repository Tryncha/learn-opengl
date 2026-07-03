#ifndef CONSTANTS_H
#define CONSTANTS_H

// clang-format off
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
// clang-format on

#endif
