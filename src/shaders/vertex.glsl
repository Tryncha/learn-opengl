#version 330 core

uniform bool isUpsideDown;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec3 ourColor;

void main() {
  if(isUpsideDown) {
    gl_Position = vec4(aPos.x, -aPos.y, aPos.z, 1.0);
  } else {
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
  }

  ourColor = aColor;
}
