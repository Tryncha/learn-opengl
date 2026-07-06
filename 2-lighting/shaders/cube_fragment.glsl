#version 330 core

in vec2 TexCoords;
in vec3 FragPosition;
in vec3 FragNormal;

out vec4 FragColor;

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

uniform Light u_Light;
uniform Material u_Material;
uniform vec3 u_ViewPosition;

vec3 calcAmbient(vec3 texDiffuse) {
  return u_Light.ambient * texDiffuse;
}

vec3 calcDiffuse(vec3 texDiffuse, vec3 lightDirection) {
  float diffuseIntensity = max(dot(FragNormal, lightDirection), 0.0);
  return u_Light.diffuse * (diffuseIntensity * texDiffuse);
}

vec3 calcSpecular(vec3 texSpecular, vec3 lightDirection) {
  vec3 viewDirection = normalize(u_ViewPosition - FragPosition);
  vec3 reflectDirection = reflect(-lightDirection, FragNormal);

  float specularIntensity = pow(max(dot(viewDirection, reflectDirection), 0.0), u_Material.shininess);

  return u_Light.specular * (specularIntensity * texSpecular);
}

float calcSpotlightIntensity(vec3 lightDirection) {
  float theta = dot(lightDirection, normalize(-u_Light.direction));
  float epsilon = u_Light.innerCutOff - u_Light.outerCutOff;

  return clamp((theta - u_Light.outerCutOff) / epsilon, 0.0, 1.0);
}

float calcAttenuation() {
  float distance = length(u_Light.position - FragPosition);
  return 1.0 / (u_Light.constant + u_Light.linear * distance + u_Light.quadratic * (distance * distance));
}

void main() {
  vec3 texDiffuse = vec3(texture(u_Material.diffuse, TexCoords));
  vec3 texSpecular = vec3(texture(u_Material.specular, TexCoords));
  vec3 lightDirection = normalize(u_Light.position - FragPosition);

  // calculate lights
  vec3 ambient = calcAmbient(texDiffuse);
  vec3 diffuse = calcDiffuse(texDiffuse, lightDirection);
  vec3 specular = calcSpecular(texSpecular, lightDirection);

  // spotlight calculations
  float spotlightIntensity = calcSpotlightIntensity(lightDirection);

  // we'll leave ambient unaffected so we always have a little light
  diffuse *= spotlightIntensity;
  specular *= spotlightIntensity;

  // light attenuation 
  float attenuation = calcAttenuation();

  ambient *= attenuation;
  diffuse *= attenuation;
  specular *= attenuation;

  // applying the light to the fragment
  vec3 finalLight = ambient + diffuse + specular;
  FragColor = vec4(finalLight, 1.0);
}
