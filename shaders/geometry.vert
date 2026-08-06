#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

uniform bool u_InvertedNormals;

void main() {
  vec4 viewPos = u_View * u_Model * vec4(a_Position, 1.0);

  FragPos = viewPos.xyz;
  TexCoords = a_TexCoords;

  mat3 normalMatrix = transpose(inverse(mat3(u_View * u_Model)));
  Normal = normalize(normalMatrix * (u_InvertedNormals ? -a_Normal : a_Normal));

  gl_Position = u_Projection * viewPos;
}
