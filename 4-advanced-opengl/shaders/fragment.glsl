#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D u_Texture1;

const float NEAR = 0.1;
const float FAR = 100.0;

float linearizeDepth(float depth) {
  float z = depth * 2.0 - 1.0;
  return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}

void main() {
  float linearDepth = linearizeDepth(gl_FragCoord.z) / FAR;
  FragColor = vec4(vec3(linearDepth), 1.0);
}
