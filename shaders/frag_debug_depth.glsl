#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D u_DepthMap;
uniform float u_NearPlane;
uniform float u_FarPlane;

// Required when using a perspective projection matrix
float linearizeDepth(float depth) {
  // Back to NDC
  float z = (2.0 * depth) - 1.0;
  return (2.0 * u_NearPlane * u_FarPlane) / (u_FarPlane + u_NearPlane - z * (u_FarPlane - u_NearPlane));
}

void main() {
  float depthValue = texture(u_DepthMap, TexCoords).r;

  // Perspective
  // FragColor = vec4(vec3(linearizeDepth(depthValue) / u_FarPlane), 1.0);

  // Orthographic
  FragColor = vec4(vec3(depthValue), 1.0);
}
