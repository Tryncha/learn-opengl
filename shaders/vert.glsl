#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoords;

out OUT_VERT {
  vec3 FragPos;
  vec3 Normal;
  vec2 TexCoords;
} out_Vert;

uniform mat4 u_View;
uniform mat4 u_Projection;

void main() {
  out_Vert.FragPos = a_Position;
  out_Vert.Normal = a_Normal;
  out_Vert.TexCoords = a_TexCoords;
  gl_Position = u_Projection * u_View * vec4(a_Position, 1.0);
}
