#version 330 core

in vec2 TexCoords;
in vec3 FragPosition;
in vec3 FragNormal;

out vec4 FragColor;

struct Material {
  sampler2D diff;
  sampler2D spec;
  float shininess;
};

uniform Material u_Material;
uniform vec3 u_ViewPos;

struct DirLight {
  vec3 dir;
  // lights
  vec3 ambt;
  vec3 diff;
  vec3 spec;
};

struct PointLight {
  vec3 pos;
  // lights
  vec3 ambt;
  vec3 diff;
  vec3 spec;
  // attenuation values
  float constant;
  float linear;
  float quadratic;
};

struct SpotLight {
  vec3 pos;
  vec3 dir;
  // lights
  vec3 ambt;
  vec3 diff;
  vec3 spec;
  // attenuation values
  float constant;
  float linear;
  float quadratic;
  // spotlight cutoff
  float innerCutOff;
  float outerCutOff;
};

const int N_POINT_LIGHTS = 4;

uniform DirLight u_DirLight;
uniform PointLight u_PointLights[N_POINT_LIGHTS];
uniform SpotLight u_SpotLight;

vec3 calcDirLight(DirLight dirLight) {
  vec3 viewDir = normalize(u_ViewPos - FragPosition);
  vec3 lightDir = normalize(-dirLight.dir);

  // textures
  vec3 texDiff = vec3(texture(u_Material.diff, TexCoords));
  vec3 texSpec = vec3(texture(u_Material.spec, TexCoords));

  // diffuse shading
  float diffIntensity = max(dot(FragNormal, lightDir), 0.0);

  // specular shading
  vec3 reflectDir = reflect(-lightDir, FragNormal);
  float specIntensity = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);

  // results combined
  vec3 dirAmbt = dirLight.ambt * texDiff;
  vec3 dirDiff = dirLight.diff * diffIntensity * texDiff;
  vec3 dirSpec = dirLight.spec * specIntensity * texSpec;

  return (dirAmbt + dirDiff + dirSpec);
}

vec3 calcPointLight(PointLight pointLight) {
  vec3 viewDir = normalize(u_ViewPos - FragPosition);
  vec3 lightDir = normalize(pointLight.pos - FragPosition);

  // textures
  vec3 texDiff = vec3(texture(u_Material.diff, TexCoords));
  vec3 texSpec = vec3(texture(u_Material.spec, TexCoords));

  // diffuse shading
  float diffIntensity = max(dot(FragNormal, lightDir), 0.0);

  // specular shading
  vec3 reflectDir = reflect(-lightDir, FragNormal);
  float specIntensity = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);

  // results combined
  vec3 pointAmbt = pointLight.ambt * texDiff;
  vec3 pointDiff = pointLight.diff * diffIntensity * texDiff;
  vec3 pointSpec = pointLight.spec * specIntensity * texSpec;

  // attenuation
  float distance = length(pointLight.pos - FragPosition);
  float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));

  pointAmbt *= attenuation;
  pointDiff *= attenuation;
  pointSpec *= attenuation;

  return (pointAmbt + pointDiff + pointSpec);
}

vec3 calcSpotLight(SpotLight spotLight) {
  vec3 viewDir = normalize(u_ViewPos - FragPosition);
  vec3 lightDir = normalize(spotLight.pos - FragPosition);

  // textures
  vec3 texDiff = vec3(texture(u_Material.diff, TexCoords));
  vec3 texSpec = vec3(texture(u_Material.spec, TexCoords));

  // diffuse shading
  float diffIntensity = max(dot(FragNormal, lightDir), 0.0);

  // specular shading
  vec3 reflectDir = reflect(-lightDir, FragNormal);
  float specIntensity = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);

  // results combined
  vec3 spotAmbt = spotLight.ambt * texDiff;
  vec3 spotDiff = spotLight.diff * diffIntensity * texDiff;
  vec3 spotSpec = spotLight.spec * specIntensity * texSpec;

  // spotlight calcualtions
  float theta = dot(lightDir, normalize(-spotLight.dir));
  float epsilon = spotLight.innerCutOff - spotLight.outerCutOff;
  float spotIntensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);

  spotDiff *= spotIntensity;
  spotSpec *= spotIntensity;

  // attenuation
  float distance = length(spotLight.pos - FragPosition);
  float attenuation = 1.0 / (spotLight.constant + spotLight.linear * distance + spotLight.quadratic * (distance * distance));

  spotAmbt *= attenuation;
  spotDiff *= attenuation;
  spotSpec *= attenuation;

  return (spotAmbt + spotDiff + spotSpec);
}

void main() {
  vec3 lightResult = vec3(0.0);

  // 1. directional light
  lightResult += calcDirLight(u_DirLight);

  // 2. point lights
  for(int i = 0; i < N_POINT_LIGHTS; i++) {
    lightResult += calcPointLight(u_PointLights[i]);
  }

  // 3. spotlight light
  lightResult += calcSpotLight(u_SpotLight);

  // applying the light to the fragment
  FragColor = vec4(lightResult, 1.0);
}
