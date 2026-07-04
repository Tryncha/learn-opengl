#version 330 core

uniform vec3 cubeColor;
uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 viewPosition;

in vec3 FragPosition;
in vec3 FragNormal;

out vec4 FragColor;

void main() {
  // ambient light
  float ambientStrength = 0.1;
  vec3 ambientLight = ambientStrength * lightColor;

  // diffuse light
  vec3 lightDirection = normalize(lightPosition - FragPosition);

  float diffuseStrength = max(dot(FragNormal, lightDirection), 0.0);
  vec3 diffuseLight = diffuseStrength * lightColor;

  // specular light
  float specularStrength = 0.5;
  int shininess = 32;

  vec3 viewDirection = normalize(viewPosition - FragPosition);
  vec3 reflectDirection = reflect(-lightDirection, FragNormal);

  float specularFactor = pow(max(dot(viewDirection, reflectDirection), 0.0), shininess);
  vec3 specularLight = specularStrength * specularFactor * lightColor;

  FragColor = vec4((ambientLight + diffuseLight + specularLight) * cubeColor, 1.0);
}
