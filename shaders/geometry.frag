#version 330 core

layout(location = 0) out vec3 g_Position;
layout(location = 1) out vec3 g_Normal;
layout(location = 2) out vec4 g_Albedo;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

void main() {
  // Store the fragment position vector in the first gbuffer texture
  g_Position = FragPos;
  // Store per-fragment normals into the buffer
  g_Normal = normalize(Normal);
  // Store diffuse per-fragment color
  g_Albedo.rgb = vec3(0.95);
}
