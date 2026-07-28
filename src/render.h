#ifndef RENDER_H
#define RENDER_H

#include "shader.h"

namespace meshes {
inline unsigned int planeVAO{};
inline unsigned int cubeVAO{};
inline unsigned int cubeVBO{};
inline unsigned int quadVAO{};
inline unsigned int quadVBO{};
}  // namespace meshes

// Renders a 1x1 3D cube in NDC
void renderCube();

// Renders a 1x1 XY quad in NDC
void renderQuad();

// Renders the 3D scene
void renderScene(const Shader& shader);

#endif
