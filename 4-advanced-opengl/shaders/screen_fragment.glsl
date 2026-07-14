#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D u_ScreenTexture1;

void main() {
  // Normal
  FragColor = vec4(vec3(texture(u_ScreenTexture1, TexCoords)), 1.0);

  // Inverse
  // FragColor = vec4(vec3(1.0 - texture(u_ScreenTexture1, TexCoords)), 1.0);

  // Grayscale
  // FragColor = texture(u_ScreenTexture1, TexCoords);
  // float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
  // FragColor = vec4(vec3(average), 1.0);

  // "Realistic" grayscale
  // FragColor = texture(u_ScreenTexture1, TexCoords);
  // float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
  // FragColor = vec4(vec3(average), 1.0);
}
