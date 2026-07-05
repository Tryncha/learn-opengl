#version 330 core

struct Light {
  vec3 position;
  vec3 direction;

  // lights
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  // attenuation values
  float constant;
  float linear;
  float quadratic;

  // spotlight cutoff
  float innerCutOff;
  float outerCutOff;
};

struct Material {
  sampler2D diffuse;
  sampler2D specular;
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

  vec3 lightDirection = normalize(light.position - FragPosition);

  // spotlight calcualtions
  float theta = dot(lightDirection, normalize(-light.direction));
  float epsilon = light.innerCutOff - light.outerCutOff;
  float spotlightIntensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0); 

  // ambient light
  vec3 ambient = light.ambient * texDiffuse;

  // diffuse light
  float diffuseIntensity = max(dot(FragNormal, lightDirection), 0.0);
  vec3 diffuse = light.diffuse * (diffuseIntensity * texDiffuse);

  // specular light
  vec3 viewDirection = normalize(viewPosition - FragPosition);
  vec3 reflectDirection = reflect(-lightDirection, FragNormal);

  float specularIntensity = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
  vec3 specular = light.specular * (specularIntensity * texSpecular);

  // we'll leave ambient unaffected so we always have a little light
  diffuse *= spotlightIntensity;
  specular *= spotlightIntensity;

  // light attenuation 
  float distance = length(light.position - FragPosition);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;

  // applying the light to the fragment
  vec3 finalLight = ambient + diffuse + specular;
  FragColor = vec4(finalLight, 1.0);
}
