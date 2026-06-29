#version 330 core

uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

in vec3 ourColor;
in vec2 texCoord;

out vec4 FragColor;

void main() {
  // linearly interpolate between both textures (80% container, 20% awesomeface)
  FragColor = mix(texture(ourTexture1, texCoord), texture(ourTexture2, texCoord), 0.2);
}
