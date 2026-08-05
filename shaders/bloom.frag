#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D u_Scene;
uniform sampler2D u_BloomBlur;

uniform bool u_IsBloomEnable;
uniform float u_Exposure;

const float GAMMA = 2.2;

void main() {
  vec3 hdrColor = texture(u_Scene, TexCoords).rgb;
  vec3 bloomColor = texture(u_BloomBlur, TexCoords).rgb;

  // Additive blending
  if (u_IsBloomEnable) {
    hdrColor += bloomColor;
  }

  // Tone mapping
  vec3 result = vec3(1.0) - exp(-hdrColor * u_Exposure);
  // also gamma correct while we're at it
  result = pow(result, vec3(1.0 / GAMMA));
  FragColor = vec4(result, 1.0);
}
