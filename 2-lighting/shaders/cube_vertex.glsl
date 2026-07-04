#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 viewPosition;

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

out vec3 AmbientLight;
out vec3 DiffuseLight;
out vec3 SpecularLight;

void main() {
  gl_Position = projection * view * model * vec4(aPosition, 1.0);

  // Gourand shading
  mat3 normalMat = mat3(transpose(inverse(model)));

  vec3 fragPosition = vec3(model * vec4(aPosition, 1.0));
  vec3 fragNormal = normalMat * aNormal;

  // ambient light
  float ambientStrength = 0.05;
  AmbientLight = ambientStrength * lightColor;

  // diffuse light
  vec3 lightDirection = normalize(lightPosition - fragPosition);

  float diffuseStrength = max(dot(fragNormal, lightDirection), 0.0);
  DiffuseLight = diffuseStrength * lightColor;

  // specular light
  // saet higher to better show the effect of Gourand shading
  float specularStrength = 1.0;
  int shininess = 32;

  vec3 viewDirection = normalize(viewPosition - fragPosition);
  vec3 reflectDirection = reflect(-lightDirection, fragNormal);

  float specularFactor = pow(max(dot(viewDirection, reflectDirection), 0.0), shininess);
  SpecularLight = specularStrength * specularFactor * lightColor;
}
