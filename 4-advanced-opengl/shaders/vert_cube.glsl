#version 330 core

layout (location = 0) in vec3 a_Position;

layout (std140) uniform ub_Matrices {
  uniform mat4 u_Projection;
  uniform mat4 u_View;
};

uniform mat4 u_Model;

void main() {
  gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
}
