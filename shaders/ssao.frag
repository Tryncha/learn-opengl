#version 330 core

in vec2 TexCoords;

out float FragColor;

uniform sampler2D u_gPosition;
uniform sampler2D u_gNormal;
uniform sampler2D u_TexNoise;

uniform vec3 u_Samples[64];

// Parameters:
// Probably want to use them as uniforms to more
// easily tweak the effect
const int KERNEL_SIZE = 64;
const float RADIUS = 0.5;
const float BIAS = 0.025;

// Tile noise texture over screen based on screen
// dimensions divided by noise size
const vec2 NOISE_SCALE = vec2(800.0 / 4.0, 600.0 / 4.0);

uniform mat4 u_Projection;

void main() {
  // Get input for SSAO algorithm
  vec3 fragPos = texture(u_gPosition, TexCoords).xyz;
  vec3 normal = normalize(texture(u_gNormal, TexCoords).rgb);
  vec3 randomVec = normalize(texture(u_TexNoise, TexCoords * NOISE_SCALE).xyz);

  // Create TBN change-of-basis matrix: from tangent-space to view-space
  vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
  vec3 bitangent = cross(normal, tangent);
  mat3 TBN = mat3(tangent, bitangent, normal);

  // Iterate over the sample kernel and calculate occlusion factor
  float occlusion = 0.0;

  for (int i = 0; i < KERNEL_SIZE; i++) {
    // Get sample position
    // From tangent to view-space
    vec3 samplePos = TBN * u_Samples[i];
    samplePos = fragPos + samplePos * RADIUS;

    // Project sample position (to sample texture)
    // (to get position on screen/texture)
    vec4 offset = vec4(samplePos, 1.0);

    // From view to clip-space
    offset = u_Projection * offset;
    // Perspective divide
    offset.xyz /= offset.w;
    // Transform to range 0.0 - 1.0
    offset.xyz = offset.xyz * 0.5 + 0.5;

    // Get sample depth:
    // Get depth value of kernel sample
    float sampleDepth = texture(u_gPosition, offset.xy).z;

    // Range check & accumulate
    float rangeCheck =
        smoothstep(0.0, 1.0, RADIUS / abs(fragPos.z - sampleDepth));

    occlusion += (sampleDepth >= samplePos.z + BIAS ? 1.0 : 0.0) * rangeCheck;
  }

  occlusion = 1.0 - (occlusion / KERNEL_SIZE);
  FragColor = occlusion;
}
