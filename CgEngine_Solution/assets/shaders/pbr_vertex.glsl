#version 450 core

layout (binding = 0, std140) uniform CameraData {
    mat4 viewProjection;
    mat4 projection;
    mat4 view;
    vec3 position;
} u_CameraData;

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;

out VS_OUT {
    vec3 WorldPosition;
    vec3 Normal;
    vec2 TexCoord;
} vs_out;

uniform mat4 u_model;

void main()
{
    vs_out.TexCoord = a_TexCoord;
    vs_out.WorldPosition = vec3(u_model * vec4(a_Pos, 1.0));
    vs_out.Normal = mat3(transpose(inverse(u_model))) * a_Normal;

    gl_Position = u_CameraData.viewProjection * u_model * vec4(a_Pos.x, a_Pos.y, a_Pos.z, 1.0);
}
