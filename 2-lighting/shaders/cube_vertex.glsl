#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;

out vec3 FragPosition;
out vec3 FragNormal;
out vec2 TexCoords;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main() {
  gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
  FragPosition = vec3(u_Model * vec4(a_Position, 1.0));

  mat3 normalMat = mat3(transpose(inverse(u_Model)));

  FragNormal = normalMat * a_Normal;
  TexCoords = a_TexCoords;
}
