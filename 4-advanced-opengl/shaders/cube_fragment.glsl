#version 330 core

in vec3 Position;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 u_CameraPosition;
uniform samplerCube u_SkyboxTexture;

void main() {
  vec3 I = normalize(Position - u_CameraPosition);
  vec3 R = reflect(I, normalize(Normal));

  FragColor = vec4(texture(u_SkyboxTexture, R).rgb, 1.0);
}
