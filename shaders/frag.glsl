#version 330 core

out vec4 FragColor;

in OUT_VERT {
  vec3 FragPos;
  vec2 TexCoords;
  vec3 TangentLightPos;
  vec3 TangentViewPos;
  vec3 TangentFragPos;
} in_Frag;

uniform sampler2D u_DiffuseMap;
uniform sampler2D u_NormalMap;
uniform sampler2D u_DepthMap;

uniform float u_HeightScale;

const int MIN_LAYERS = 8;
const int MAX_LAYERS = 32;

vec2 calcParallaxMapping(vec3 viewDir) {
  // Calculate the number of layers
  float numLayers = mix(MAX_LAYERS, MIN_LAYERS, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
  // Calculate the size of each layer
  float layerDepth = 1.0 / numLayers;
  // Depth of current layer
  float currentLayerDepth = 0.0;
  // Amount to shift the texture coordinates per layer (from vector P)
  vec2 P = viewDir.xy / viewDir.z * u_HeightScale;
  vec2 deltaTexCoords = P / numLayers;

  // Get initial values
  vec2 currentTexCoords = in_Frag.TexCoords;
  float currentDepthMapValue = texture(u_DepthMap, currentTexCoords).r;

  while (currentLayerDepth < currentDepthMapValue) {
    // Shift texture coordinates along direction of P
    currentTexCoords -= deltaTexCoords;
    // Get depth map value at current texture coordinates
    currentDepthMapValue = texture(u_DepthMap, currentTexCoords).r;
    // Get depth of next layer
    currentLayerDepth += layerDepth;
  }

  return currentTexCoords;
}

void main() {
  vec3 viewDir = normalize(in_Frag.TangentViewPos - in_Frag.TangentFragPos);
  vec3 lightDir = normalize(in_Frag.TangentLightPos - in_Frag.TangentFragPos);

  vec2 texCoords = calcParallaxMapping(viewDir);
  if (texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0) {
    discard;
  }

  // Obtain normal from normal map in range [0,1]
  vec3 normal = texture(u_NormalMap, texCoords).rgb;
  // Transform normal vector to range [-1,1]
  normal = normalize(normal * 2.0 - 1.0);
  // Get diffuse color
  vec3 diffColor = texture(u_DiffuseMap, texCoords).rgb;

  // Ambient
  vec3 ambient = 0.1 * diffColor;

  // Diffuse
  float diffIntensity = max(dot(lightDir, normal), 0.0);
  vec3 diffuse = diffIntensity * diffColor;

  // Specular
  float specIntensity = 0.0;
  vec3 halfwayDir = normalize(lightDir + viewDir);
  specIntensity = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
  vec3 specular = vec3(0.2) * specIntensity;

  vec3 lightResult = ambient + diffuse + specular;
  FragColor = vec4(lightResult, 1.0);
}
