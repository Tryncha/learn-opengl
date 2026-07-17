#version 330 core

in vec3 Position;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 u_CameraPosition;
uniform samplerCube u_SkyboxTexture;

const float AIR_REFRACTIVE_INDEX = 1.0;
const float GLASS_REFRACTIVE_INDEX = 1.52;

vec4 reflectColor() {
  vec3 I = normalize(Position - u_CameraPosition);
  vec3 R = reflect(I, normalize(Normal));

  return vec4(texture(u_SkyboxTexture, R).rgb, 1.0);
}

vec4 refractColor() {
  float ratio = AIR_REFRACTIVE_INDEX / GLASS_REFRACTIVE_INDEX;
  vec3 I = normalize(Position - u_CameraPosition);
  vec3 R = refract(I, normalize(Normal), ratio);

  return vec4(texture(u_SkyboxTexture, R).rgb, 1.0);
}

void main() {
  FragColor = refractColor();
}
