#version 330 core

out vec4 FragColor;

in OUT_VERT {
  vec3 FragPos;
  vec3 Normal;
  vec2 TexCoords;
} in_Frag;

uniform sampler2D u_DiffuseTexture;
uniform samplerCube u_DepthMap;

uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;

uniform float u_FarPlane;

const float BIAS = 0.05;
const float SAMPLES = 4.0;
const float OFFSET = 0.1;

float calcShadowIntensity() {
  // Get vector between fragment position and light position
  vec3 fragToLight = in_Frag.FragPos - u_LightPos;

  // ise the fragment to light vector to sample from the depth map    
  float closestDepth = texture(u_DepthMap, fragToLight).r;
  // It is currently in linear range between [0,1], let's re-transform it back to original depth value
  closestDepth *= u_FarPlane;

  // Now get current linear depth as the length between the fragment and light position
  float currentDepth = length(fragToLight);

  // Now calculate shadows using PCF
  float shadowIntensity = 0.0;

  for (float x = -OFFSET; x < OFFSET; x += OFFSET / (SAMPLES * 0.5)) {
    for (float y = -OFFSET; y < OFFSET; y += OFFSET / (SAMPLES * 0.5)) {
      for (float z = -OFFSET; z < OFFSET; z += OFFSET / (SAMPLES * 0.5)) {
        float closestDepth = texture(u_DepthMap, fragToLight + vec3(x, y, z)).r;
        // Undo mapping [0,1]
        closestDepth *= u_FarPlane;
        if (currentDepth - BIAS > closestDepth) {
          shadowIntensity += 1.0;
        }
      }
    }
  }

  shadowIntensity /= (SAMPLES * SAMPLES * SAMPLES);
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
  float shadowIntensity = calcShadowIntensity();
  vec3 lightResult = (ambient + ((1.0 - shadowIntensity) * (diffuse + specular))) * texColor;

  FragColor = vec4(lightResult, 1.0);
}
