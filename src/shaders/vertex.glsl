#version 330 core

uniform mat4 transf;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 texCoord;

void main() {
  gl_Position = transf * vec4(aPos.x, aPos.y, aPos.z, 1.0);
  texCoord = vec2(aTexCoord.x, aTexCoord.y);
}
