#version 450 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

layout (binding = 0, std140) uniform CameraData {
    mat4 viewProjection;
    mat4 projection;
    mat4 view;
    vec3 position;
} u_CameraData;

in VS_OUT {
    vec3 Normal;
} fs_in[];

void generateLine(int index) {
    gl_Position = u_CameraData.projection * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = u_CameraData.projection * (gl_in[index].gl_Position + vec4(fs_in[index].Normal, 0.0) * 0.2);
    EmitVertex();
    EndPrimitive();
}

void main() {
    generateLine(0);
    generateLine(1);
    generateLine(2);
}
