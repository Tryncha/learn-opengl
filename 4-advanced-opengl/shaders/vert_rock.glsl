#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 2) in vec2 a_TexCoords;
layout (location = 3) in mat4 a_InstanceMatrix;

out vec2 TexCoords;

uniform mat4 u_View;
uniform mat4 u_Projection;

void main() {
  gl_Position = u_Projection * u_View * a_InstanceMatrix * vec4(a_Position, 1.0);
  TexCoords = a_TexCoords;
}
