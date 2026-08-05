#version 330 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

in OUT_VERT {
  vec3 FragPos;
  vec3 Normal;
  vec2 TexCoords;
}
in_Frag;

struct Light {
  vec3 position;
  vec3 color;
};

uniform Light u_Lights[4];
uniform sampler2D u_DiffuseTexture;

void main() {
  vec3 diffTexColor = texture(u_DiffuseTexture, in_Frag.TexCoords).rgb;

  // Lighting
  vec3 lightResult = vec3(0.0);

  for (int i = 0; i < 4; i++) {
    // Diffuse
    vec3 lightDir = normalize(u_Lights[i].position - in_Frag.FragPos);
    float diffIntensity = max(dot(lightDir, in_Frag.Normal), 0.0);
    vec3 diffuseResult = u_Lights[i].color * diffIntensity * diffTexColor;

    // Attenuation (use quadratic as we have gamma correction)
    float distance = length(in_Frag.FragPos - u_Lights[i].position);
    diffuseResult *= 1.0 / (distance * distance);
    lightResult += diffuseResult;
  }

  // Check whether result is higher than some threshold,
  // if so, output as bloom threshold color float brightness = dot(result,
  // vec3(0.2126, 0.7152, 0.0722));
  float brightness = dot(lightResult, vec3(0.2126, 0.7152, 0.0722));

  if (brightness > 1.0) {
    BrightColor = vec4(lightResult, 1.0);
  } else {
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
  }

  FragColor = vec4(lightResult, 1.0);
}
