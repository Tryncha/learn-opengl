#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 FragPosition;
out vec3 FragNormal;
out vec2 TexCoords;

void main() {
  gl_Position = projection * view * model * vec4(aPosition, 1.0);
  FragPosition = vec3(model * vec4(aPosition, 1.0));

  mat3 normalMat = mat3(transpose(inverse(model)));

  FragNormal = normalMat * aNormal;
  TexCoords = aTexCoords;
}
