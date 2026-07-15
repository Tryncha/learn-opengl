#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D u_ScreenTexture1;

const float OFFSET = 1.0 / 300.0;

const vec2 OFFSETS[9] = vec2[](
  vec2(-OFFSET, OFFSET),
  vec2(0.0,     OFFSET),
  vec2(OFFSET,  OFFSET),
  vec2(-OFFSET, 0.0),
  vec2(0.0,     0.0),
  vec2(OFFSET,  0.0),
  vec2(-OFFSET, -OFFSET),
  vec2(0.0,     -OFFSET),
  vec2(OFFSET,  -OFFSET)
);

const float KERNEL[9] = float[](
  -1, -1, -1,
  -1,  9, -1,
  -1, -1, -1
);

void main() {
  vec3 sampleTex[9];
  for (int i = 0; i < 9; i++) {
    sampleTex[i] = vec3(texture(u_ScreenTexture1, TexCoords.st + OFFSETS[i]));
  }

  vec3 color = vec3(0.0);
  for (int i = 0; i < 9; i++) {
    color += sampleTex[i] * KERNEL[i];
  }

  FragColor = vec4(color, 1.0);
}
