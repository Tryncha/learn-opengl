#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VERTEX_OUT {
  vec2 TexCoords;
} geometry_in[];

out vec2 TexCoords;

void main() {
  gl_Position = gl_in[0].gl_Position;
  TexCoords = geometry_in[0].TexCoords;
  EmitVertex();

  gl_Position = gl_in[1].gl_Position;
  TexCoords = geometry_in[1].TexCoords;
  EmitVertex();

  gl_Position = gl_in[2].gl_Position;
  TexCoords = geometry_in[2].TexCoords;
  EmitVertex();

  EndPrimitive();
}
