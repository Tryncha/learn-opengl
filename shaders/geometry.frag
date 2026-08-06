#version 330 core

layout(location = 0) out vec3 g_Position;
layout(location = 1) out vec3 g_Normal;
layout(location = 2) out vec4 g_AlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D u_TextureDiffuse1;
uniform sampler2D u_TextureSpecular1;

void main() {
  // Store the fragment position vector in the first gbuffer texture
  g_Position = FragPos;

  // Store per-fragment normals into the buffer
  g_Normal = normalize(Normal);

  // Store diffuse per-fragment color
  g_AlbedoSpec.rgb = texture(u_TextureDiffuse1, TexCoords).rgb;

  // Store specular intensity in g_AlbedoSpec's alpha component
  g_AlbedoSpec.a = texture(u_TextureSpecular1, TexCoords).r;
}
