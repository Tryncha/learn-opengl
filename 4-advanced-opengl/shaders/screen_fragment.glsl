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

const float SHARPEN_KERNEL[9] = float[](
  -1.0, -1.0, -1.0,
  -1.0,  9.0, -1.0,
  -1.0, -1.0, -1.0
);

const float BLUR_KERNEL[9] = float[](
  1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
  2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
  1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0
);

const float EDGE_DET_KERNEL[9] = float[](
  1.0,  1.0, 1.0,
  1.0, -9.0, 1.0,
  1.0,  1.0, 1.0
);

void main() {
  vec3 sampleTex[9];
  for (int i = 0; i < 9; i++) {
    sampleTex[i] = vec3(texture(u_ScreenTexture1, TexCoords.st + OFFSETS[i]));
  }

  vec3 color = vec3(0.0);
  for (int i = 0; i < 9; i++) {
    color += sampleTex[i] * BLUR_KERNEL[i];
  }

  FragColor = vec4(color, 1.0);
}
