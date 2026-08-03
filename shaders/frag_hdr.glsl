#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D u_HdrBuffer;
uniform bool u_IsHdrEnable;
uniform float u_Exposure;

const float GAMMA = 2.2;

void main() {
  vec3 hdrColor = texture(u_HdrBuffer, TexCoords).rgb;

  if (u_IsHdrEnable) {
    // Reinhard tone mapping
    // vec3 result = hdrColor / (hdrColor + vec3(1.0));

    // Exposure tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * u_Exposure);

    // Gamma correction
    result = pow(result, vec3(1.0 / GAMMA));
    FragColor = vec4(result, 1.0);
  } else {
    vec3 result = pow(hdrColor, vec3(1.0 / GAMMA));
    FragColor = vec4(result, 1.0);
  }
}
