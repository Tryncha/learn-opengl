#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 2) in vec2 a_TexCoords;

out VERTEX_OUT {
  out vec2 TexCoords;
} vertex_out;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main() {
  gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
  vertex_out.TexCoords = a_TexCoords;
}
