#version 450 core

layout (binding = 0, std140) uniform CameraData {
    mat4 viewProjection;
    mat4 projection;
    mat4 view;
    vec3 position;
} u_CameraData;

layout(binding = 0, std430) buffer Transforms {
    mat4 transforms[];
} b_Transforms;

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in vec3 a_Tangent;
layout (location = 4) in vec3 a_Bitangent;

out VS_OUT {
    vec3 Normal;
} vs_out;

void main() {
    mat4 model = b_Transforms.transforms[gl_InstanceID];
    vs_out.Normal = normalize(mat3(transpose(inverse(u_CameraData.view * model))) * a_Normal);
    gl_Position = u_CameraData.view * model * vec4(a_Pos, 1.0f);
}
