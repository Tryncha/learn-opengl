#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoords;

out OUT_VERT {
  vec3 FragPos;
  vec3 Normal;
  vec2 TexCoords;
} out_Vert;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

uniform bool u_InverseNormals;

void main() {
  out_Vert.FragPos = vec3(u_Model * vec4(a_Position, 1.0));
  out_Vert.TexCoords = a_TexCoords;

  mat3 normalMatrix = transpose(inverse(mat3(u_Model)));
  out_Vert.Normal = normalize(normalMatrix * (u_InverseNormals ? -a_Normal : a_Normal));

  gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
}
