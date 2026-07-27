#version 330 core

out vec4 FragColor;

in OUT_VERT {
  vec3 FragPos;
  vec3 Normal;
  vec2 TexCoords;
} in_Frag;

uniform sampler2D u_Texture1;
uniform vec3 u_ViewPos;
uniform vec3 u_LightPos;
uniform bool u_IsBlinnActive;

void main() {
  // Pre-calcs
  vec3 viewDir = normalize(u_ViewPos - in_Frag.FragPos);
  vec3 lightDir = normalize(u_LightPos - in_Frag.FragPos);
  vec3 texColor = vec3(texture(u_Texture1, in_Frag.TexCoords));

  // Ambient
  vec3 ambient = 0.05 * texColor;

  // Diffuse
  float diffIntensity = max(dot(lightDir, in_Frag.Normal), 0.0);
  vec3 diffuse = diffIntensity * texColor;

  // Specular
  float specIntensity = 0.0;

  if (u_IsBlinnActive) {
    vec3 halfwayDir = normalize(lightDir + viewDir);
    specIntensity = pow(max(dot(in_Frag.Normal, halfwayDir), 0.0), 32.0);
  } else {
    vec3 reflectDir = reflect(-lightDir, in_Frag.Normal);
    specIntensity = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
  }

  // Assuming bright white light color
  vec3 specular = vec3(0.3) * specIntensity;

  FragColor = vec4(ambient + diffuse + specular, 1.0);
}
