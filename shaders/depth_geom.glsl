#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

// FragPos from GS (output per emitvertex)
out vec4 FragPos;

uniform mat4 u_ShadowTransf[6];

void main() {
  for (int face = 0; face < 6; face++) {
    // Built-in variable that specifies to which face we render.
    gl_Layer = face;

    // For each triangle's vertices:
    for (int i = 0; i < 3; i++) {
      FragPos = gl_in[i].gl_Position;
      gl_Position = u_ShadowTransf[face] * FragPos;
      EmitVertex();
    }

    EndPrimitive();
  }
}
