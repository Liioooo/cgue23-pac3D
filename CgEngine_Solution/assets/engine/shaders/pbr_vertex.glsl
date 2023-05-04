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

layout (binding = 2, std140) uniform DirShadowData {
    mat4 lightSpaceMat[4];
    vec4 cascadeSplits;
} u_DirShadowData;

layout(binding = 0, std430) buffer Transforms {
    mat4 transforms[];
} b_Transforms;

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in vec3 a_Tangent;
layout (location = 4) in vec3 a_Bitangent;

out VS_OUT {
    vec3 WorldPosition;
    vec4 DirShadowMapPosition[4];
    vec3 Normal;
    mat3 TBN;
    vec2 TexCoord;
} vs_out;

void main() {
    mat4 model = b_Transforms.transforms[gl_InstanceID];

    vec4 worldPosition = model * vec4(a_Pos, 1.0f);

    vs_out.TexCoord = a_TexCoord;
    vs_out.WorldPosition = worldPosition.xyz;
    vs_out.Normal = mat3(transpose(inverse(model))) * a_Normal;
    vs_out.TBN = mat3(model) * mat3(a_Tangent, a_Bitangent, a_Normal);
    vs_out.DirShadowMapPosition[0] = u_DirShadowData.lightSpaceMat[0] * vec4(worldPosition.xyz, 1.0f);
    vs_out.DirShadowMapPosition[1] = u_DirShadowData.lightSpaceMat[1] * vec4(worldPosition.xyz, 1.0f);
    vs_out.DirShadowMapPosition[2] = u_DirShadowData.lightSpaceMat[2] * vec4(worldPosition.xyz, 1.0f);
    vs_out.DirShadowMapPosition[3] = u_DirShadowData.lightSpaceMat[3] * vec4(worldPosition.xyz, 1.0f);

    gl_Position = u_CameraData.viewProjection * worldPosition;
}
