#version 450 core

layout (binding = 0, std140) uniform CameraData {
    mat4 viewProjection;
    mat4 projection;
    mat4 view;
    vec3 position;
    float exposure;
    float bloomIntensity;
    float bloomThreshold;
} u_CameraData;

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Color;

out VS_OUT {
    vec3 Color;
} vs_out;

void main() {
    gl_Position = u_CameraData.viewProjection * vec4(a_Pos, 1.0f);
    vs_out.Color = a_Color;
}