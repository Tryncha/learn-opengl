#version 330 core

out vec4 FragColor;

in OUT_VERT {
  vec3 FragPos;
  vec3 Normal;
  vec2 TexCoords;
  vec4 FragPosLightSpace;
} in_Frag;

uniform sampler2D u_DiffuseTexture;
uniform sampler2D u_ShadowMap;

uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;

bool calcShadow(vec3 lightDir) {
  // Perform perspective divide
  vec3 projCoords = in_Frag.FragPosLightSpace.xyz / in_Frag.FragPosLightSpace.w;
  // Transform to [0,1] range
  projCoords = projCoords * 0.5 + 0.5;

  // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
  float closestDepth = texture(u_ShadowMap, projCoords.xy).r; 
  // Get depth of current fragment from light's perspective
  float currentDepth = projCoords.z;

  // Calculate the bias
  float bias = max(0.05 * (1.0 - dot(in_Frag.Normal, lightDir)), 0.005);
  // Check whether current frag pos is in shadow
  bool isShadow = (currentDepth - bias) > closestDepth;

  // Force the shadow value to 0.0 whenever the projected
  // vector's z-coord is larger than 1.0
  if (projCoords.z > 1.0)
    isShadow = false;

  return isShadow;
}

float calcShadowPCF(vec3 lightDir) {
  // Perform perspective divide
  vec3 projCoords = in_Frag.FragPosLightSpace.xyz / in_Frag.FragPosLightSpace.w;
  // Transform to [0,1] range
  projCoords = projCoords * 0.5 + 0.5;

  // Get depth of current fragment from light's perspective
  float currentDepth = projCoords.z;

  // Calculate the bias
  float bias = max(0.05 * (1.0 - dot(in_Frag.Normal, lightDir)), 0.005);

  // Check whether current frag pos is in shadow
  float shadowIntensity = 0.0;
  vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);

  for (int x = -1; x <= 1; x++) {
    for (int y = -1; y <= 1; y++) {
      float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
      shadowIntensity += (currentDepth - bias > pcfDepth) ? 1.0 : 0.0;
    }
  }

  // Reduce intensity to soften the shadow
  shadowIntensity /= 9.0;

  // Force the shadow value to 0.0 whenever the projected
  // vector's z-coord is larger than 1.0
  if (projCoords.z > 1.0)
    shadowIntensity = 0.0;

  return shadowIntensity;
}

void main() {
  vec3 viewDir = normalize(u_ViewPos - in_Frag.FragPos);
  vec3 lightDir = normalize(u_LightPos - in_Frag.FragPos);
  vec3 lightColor = vec3(0.3);
  vec3 texColor = texture(u_DiffuseTexture, in_Frag.TexCoords).rgb;

  // Ambient
  vec3 ambient = 0.3 * lightColor;

  // Diffuse
  float diffIntensity = max(dot(lightDir, in_Frag.Normal), 0.0);
  vec3 diffuse = diffIntensity * lightColor;

  // Specular
  float specIntensity = 0.0;
  vec3 halfwayDir = normalize(lightDir + viewDir);
  specIntensity = pow(max(dot(in_Frag.Normal, halfwayDir), 0.0), 64.0);
  vec3 specular = specIntensity * lightColor;  

  // Calculate shadow
  float shadowIntensity = calcShadowPCF(lightDir);
  vec3 lightResult = (ambient + ((1.0 - shadowIntensity) * (diffuse + specular))) * texColor;

  FragColor = vec4(lightResult, 1.0);
}
