#version 330 core

out vec4 FragColor;

in OUT_VERT {
  vec3 FragPos;
  vec3 Normal;
  vec2 TexCoords;
} in_Frag;

struct Light {
  vec3 position;
  vec3 color;
};

uniform Light u_Lights[16];
uniform sampler2D u_DiffuseTexture;

void main() {
  vec3 diffTexColor = texture(u_DiffuseTexture, in_Frag.TexCoords).rgb;

  // Lighting
  vec3 lightResult = vec3(0.0);

  for (int i = 0; i < 16; i++) {
    // Diffuse
    vec3 lightDir = normalize(u_Lights[i].position - in_Frag.FragPos);
    float diffIntensity = max(dot(lightDir, in_Frag.Normal), 0.0);
    vec3 diffuseResult = u_Lights[i].color * diffIntensity * diffTexColor;

    // Attenuation (use quadratic as we have gamma correction)
    float distance = length(in_Frag.FragPos - u_Lights[i].position);
    diffuseResult *= 1.0 / (distance * distance);
    lightResult += diffuseResult;
  }

  FragColor = vec4(lightResult, 1.0);
}
