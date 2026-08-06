#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D u_gPosition;
uniform sampler2D u_gNormal;
uniform sampler2D u_gAlbedo;
uniform sampler2D u_SSAO;

struct Light {
  vec3 position;
  vec3 color;

  float linear;
  float quadratic;
};

uniform Light u_Light;

void main() {
  // Retrieve data from gbuffer
  vec3 gFragPos = texture(u_gPosition, TexCoords).rgb;
  vec3 gNormal = texture(u_gNormal, TexCoords).rgb;
  vec3 gDiffuse = texture(u_gAlbedo, TexCoords).rgb;
  float ambientOcclusion = texture(u_SSAO, TexCoords).r;

  // Then calculate lighting as usual
  vec3 ambient = vec3(0.3 * gDiffuse * ambientOcclusion);
  vec3 lightResult = ambient;

  // u_ViewPos is (0.0.0)
  vec3 viewDir = normalize(-gFragPos);

  // Diffuse
  vec3 lightDir = normalize(u_Light.position - gFragPos);
  vec3 diffuse = max(dot(gNormal, lightDir), 0.0) * gDiffuse * u_Light.color;

  // Specular
  vec3 halfwayDir = normalize(lightDir + viewDir);
  float spec = pow(max(dot(gNormal, halfwayDir), 0.0), 8.0);
  vec3 specular = u_Light.color * spec;

  // Attenuation
  float distance = length(u_Light.position - gFragPos);
  float attenuation = 1.0 / (1.0 + u_Light.linear * distance +
                             u_Light.quadratic * distance * distance);

  diffuse *= attenuation;
  specular *= attenuation;
  lightResult += diffuse + specular;

  FragColor = vec4(lightResult, 1.0);
}
