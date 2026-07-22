#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D u_TextureDiffuse1;

void main() {
  FragColor = texture(u_TextureDiffuse1, TexCoords);
}
