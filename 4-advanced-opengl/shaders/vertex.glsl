#version 330 core

layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec3 a_Color;
layout (location = 2) in vec2 a_Offset;

out vec3 Color;

void main() {
  gl_Position = vec4(a_Position + a_Offset, 0.0, 1.0);
  Color = a_Color;
}
