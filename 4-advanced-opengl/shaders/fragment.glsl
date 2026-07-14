#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D u_Texture1;

void main() {
  vec4 texColor = texture(u_Texture1, TexCoords);

  if(texColor.a < 0.1)
    discard;

  FragColor = texColor;
}
