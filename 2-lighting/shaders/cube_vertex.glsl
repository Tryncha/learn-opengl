#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPosition;

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

out vec3 FragPosition;
out vec3 FragNormal;
out vec3 ViewLightPosition;

void main() {
  gl_Position = projection * view * model * vec4(aPosition, 1.0);

  mat3 normalMat = mat3(transpose(inverse(view * model)));

  FragPosition = vec3(view * model * vec4(aPosition, 1.0));
  FragNormal = normalMat * aNormal;

  // transform world-space light position to view-space light position
  ViewLightPosition = vec3(view * vec4(lightPosition, 1.0));
}
