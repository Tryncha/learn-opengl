#version 330 core

layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec3 a_Color;

out vec3 Color;

uniform vec2 u_Offsets[100];

void main() {
  gl_Position = vec4(a_Position + u_Offsets[gl_InstanceID], 0.0, 1.0);
  Color = a_Color;
}
