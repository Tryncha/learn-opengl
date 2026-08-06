#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D u_gPosition;
uniform sampler2D u_gNormal;
uniform sampler2D u_gAlbedoSpec;

struct Light {
  vec3 position;
  vec3 color;

  float linear;
  float quadratic;
  float radius;
};

const int N_LIGHTS = 32;

uniform Light u_Lights[N_LIGHTS];
uniform vec3 u_ViewPos;

void main() {
  // Retrieve data from gbuffer
  vec3 gFragPos = texture(u_gPosition, TexCoords).rgb;
  vec3 gNormal = texture(u_gNormal, TexCoords).rgb;
  vec3 gDiffuse = texture(u_gAlbedoSpec, TexCoords).rgb;
  float gSpecular = texture(u_gAlbedoSpec, TexCoords).a;

  // Then calculate lighting as usual
  // Hard-coded ambient component
  vec3 lightResult = gDiffuse * 0.1;
  vec3 viewDir = normalize(u_ViewPos - gFragPos);

  for (int i = 0; i < N_LIGHTS; i++) {
    // Calculate distance between light source and current fragment
    float distance = length(u_Lights[i].position - gFragPos);

    if (distance < u_Lights[i].radius) {
      // Diffuse
      vec3 lightDir = normalize(u_Lights[i].position - gFragPos);
      vec3 diffuse =
          max(dot(gNormal, lightDir), 0.0) * gDiffuse * u_Lights[i].color;

      // Specular
      vec3 halfwayDir = normalize(lightDir + viewDir);
      float spec = pow(max(dot(gNormal, halfwayDir), 0.0), 16.0);
      vec3 specular = u_Lights[i].color * spec * gSpecular;

      // Attenuation
      float attenuation = 1.0 / (1.0 + u_Lights[i].linear * distance +
                                 u_Lights[i].quadratic * distance * distance);

      diffuse *= attenuation;
      specular *= attenuation;
      lightResult += diffuse + specular;
    }
  }

  FragColor = vec4(lightResult, 1.0);
}
