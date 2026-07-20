#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VERTEX_OUT {
  vec3 Normal;
} geometry_in[];

const float MAGNITUDE = 0.2;

uniform mat4 u_Projection;

void genLine(int index) {
  gl_Position = u_Projection * gl_in[index].gl_Position;
  EmitVertex();
  gl_Position = u_Projection * (gl_in[index].gl_Position + vec4(geometry_in[index].Normal, 0.0) * MAGNITUDE);
  EmitVertex();
  EndPrimitive();
}

void main() {
  genLine(0);
  genLine(1);
  genLine(2);
}
