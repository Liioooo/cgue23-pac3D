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
layout (location = 3) in vec3 a_Tangent;
layout (location = 4) in vec3 a_Bitangent;

out VS_OUT {
    vec3 WorldPosition;
    vec3 Normal;
    vec2 TexCoord;
} vs_out;

uniform mat4 u_model;

void main()
{
    vec4 worldPosition = u_model * vec4(a_Pos, 1.0f);

    vs_out.TexCoord = a_TexCoord;
    vs_out.WorldPosition = worldPosition.xyz;
    vs_out.Normal = mat3(transpose(inverse(u_model))) * a_Normal;

    gl_Position = u_CameraData.viewProjection * worldPosition;
}
