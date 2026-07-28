#version 330 core

out vec4 FragColor;

in OUT_VERT {
  vec3 FragPos;
  vec3 Normal;
  vec2 TexCoords;
} in_Frag;

uniform sampler2D u_FloorTexture;
uniform vec3 u_ViewPos;
uniform vec3 u_LightPositions[4];
uniform vec3 u_LightColors[4];
uniform bool u_isGammaEnable;

vec3 blinnPhong(vec3 lightPos, vec3 lightColor) {
  // Pre-calcs
  vec3 viewDir = normalize(u_ViewPos - in_Frag.FragPos);
  vec3 lightDir = normalize(lightPos - in_Frag.FragPos);

  // Diffuse
  float diffIntensity = max(dot(lightDir, in_Frag.Normal), 0.0);
  vec3 diffuse = diffIntensity * lightColor;

  // Specular
  float specIntensity = 0.0;
  vec3 halfwayDir = normalize(lightDir + viewDir);
  specIntensity = pow(max(dot(in_Frag.Normal, halfwayDir), 0.0), 64.0);
  vec3 specular = specIntensity * lightColor;

  // simple attenuation
  float distance = length(lightPos - in_Frag.FragPos);
  float attenuation = 1.0 / (u_isGammaEnable ? (distance * distance) : distance);

  diffuse *= attenuation;
  specular *= attenuation;

  return diffuse + specular;
}

void main() {
  vec3 color = vec3(texture(u_FloorTexture, in_Frag.TexCoords));
  vec3 lightResult = vec3(0.0);

  for (int i = 0; i < 4; i++) {
    lightResult += blinnPhong(u_LightPositions[i], u_LightColors[i]);
  }

  color *= lightResult;

  if (u_isGammaEnable) {
    color = pow(color, vec3(1.0 / 2.2));
  }

  FragColor = vec4(color, 1.0);
}
