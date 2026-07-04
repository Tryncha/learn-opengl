#version 330 core

uniform vec3 cubeColor;

in vec3 AmbientLight;
in vec3 DiffuseLight;
in vec3 SpecularLight;

out vec4 FragColor;

void main() {
  FragColor = vec4((AmbientLight + DiffuseLight + SpecularLight) * cubeColor, 1.0);
}
