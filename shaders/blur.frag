#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D u_Image;
uniform bool u_DoHorizontal;

const float WEIGHTS[5] = float[](0.2270270270, 0.1945945946, 0.1216216216,
                                 0.0540540541, 0.0162162162);

void main() {
  // Gets size of single texel
  vec2 texOffset = 1.0 / textureSize(u_Image, 0);
  vec3 result = texture(u_Image, TexCoords).rgb * WEIGHTS[0];

  if (u_DoHorizontal) {
    for (int i = 1; i < 5; ++i) {
      result += texture(u_Image, TexCoords + vec2(texOffset.x * i, 0.0)).rgb *
                WEIGHTS[i];
      result += texture(u_Image, TexCoords - vec2(texOffset.x * i, 0.0)).rgb *
                WEIGHTS[i];
    }
  } else {
    for (int i = 1; i < 5; ++i) {
      result += texture(u_Image, TexCoords + vec2(0.0, texOffset.y * i)).rgb *
                WEIGHTS[i];
      result += texture(u_Image, TexCoords - vec2(0.0, texOffset.y * i)).rgb *
                WEIGHTS[i];
    }
  }

  FragColor = vec4(result, 1.0);
}
