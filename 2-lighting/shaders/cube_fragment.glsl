#version 330 core

struct Light {
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  sampler2D emission;
  float shininess;
};

uniform Light light;
uniform Material material;

uniform vec3 viewPosition;

in vec2 TexCoords;
in vec3 FragPosition;
in vec3 FragNormal;

out vec4 FragColor;

void main() {
  vec3 texDiffuse = vec3(texture(material.diffuse, TexCoords));
  vec3 texSpecular = vec3(texture(material.specular, TexCoords));
  vec3 texEmission = vec3(texture(material.emission, TexCoords));

  // ambient light
  vec3 ambient = light.ambient * texDiffuse;

  // diffuse light
  vec3 lightDirection = normalize(light.position - FragPosition);
  float diffuseIntensity = max(dot(FragNormal, lightDirection), 0.0);
  vec3 diffuse = light.diffuse * (diffuseIntensity * texDiffuse);

  // specular light
  vec3 viewDirection = normalize(viewPosition - FragPosition);
  vec3 reflectDirection = reflect(-lightDirection, FragNormal);

  float specularIntensity = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
  vec3 specular = light.specular * (specularIntensity * texSpecular);

  // emission light
  vec3 emission = vec3(0.0);

  if(texSpecular.x == 0.0) {
    emission = texEmission;
  }

  vec3 finalLight = ambient + diffuse + specular + emission;
  FragColor = vec4(finalLight, 1.0);
}
