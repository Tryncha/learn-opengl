#ifndef RENDER_H
#define RENDER_H

#include "shader.h"

namespace meshes {
inline unsigned int planeVAO{};
inline unsigned int cubeVAO{0};
inline unsigned int cubeVBO{0};
inline unsigned int quadVAO{0};
inline unsigned int quadVBO{};
}  // namespace meshes

// Renders a 1x1 3D cube in NDC
void renderCube();

// Renders a 1x1 XY quad in NDC
void renderQuad();

// Renders the 3D scene
void renderScene(const Shader& shader);

#endif
