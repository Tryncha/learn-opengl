#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;

out VERTEX_OUT {
  vec3 Normal;
} vertex_out;

uniform mat4 u_Model;
uniform mat4 u_View;

void main() {
  mat3 normalMatrix = mat3(transpose(inverse(u_View * u_Model)));
  vertex_out.Normal = vec3(vec4(normalMatrix * a_Normal, 0.0));
  gl_Position = u_View * u_Model * vec4(a_Position, 1.0);
}
