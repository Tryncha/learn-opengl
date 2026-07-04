#version 330 core

struct Light {
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

uniform Light light;
uniform Material material;

uniform vec3 viewPosition;

in vec3 FragPosition;
in vec3 FragNormal;

out vec4 FragColor;

void main() {
  // ambient light
  vec3 ambient = light.ambient * material.ambient;

  // diffuse light
  vec3 lightDirection = normalize(light.position - FragPosition);
  float diffuseIntensity = max(dot(FragNormal, lightDirection), 0.0);
  vec3 diffuse = light.diffuse * (diffuseIntensity * material.diffuse);

  // specular light
  vec3 viewDirection = normalize(viewPosition - FragPosition);
  vec3 reflectDirection = reflect(-lightDirection, FragNormal);

  float specularIntensity = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
  vec3 specular = light.specular * (specularIntensity * material.specular);

  FragColor = vec4(ambient + diffuse + specular, 1.0);
}
