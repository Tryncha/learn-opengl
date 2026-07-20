#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VERTEX_OUT {
  vec2 TexCoords;
} geometry_in[];

out vec2 TexCoords;

uniform float u_Time;

const float EXPLODE_FACTOR = 2.0;

vec3 calcNormal() {
  vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
  vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
  return normalize(cross(a, b));
}

vec4 explodeVert(vec4 position) {
  vec3 normal = calcNormal();
  vec3 direction = normal * ((sin(u_Time) + 1.0) / 2.0) * EXPLODE_FACTOR;
  return position + vec4(direction, 0.0);
}

void main() {
  gl_Position = explodeVert(gl_in[0].gl_Position);
  TexCoords = geometry_in[0].TexCoords;
  EmitVertex();

  gl_Position = explodeVert(gl_in[1].gl_Position);
  TexCoords = geometry_in[1].TexCoords;
  EmitVertex();

  gl_Position = explodeVert(gl_in[2].gl_Position);
  TexCoords = geometry_in[2].TexCoords;
  EmitVertex();

  EndPrimitive();
}
