#version 450 core

#include "common/CameraDataBuffer.glsl"

layout(binding = 0, std430) buffer Transforms {
    mat4 transforms[];
} b_Transforms;

layout (location = 0) in vec4 a_Pos;
layout (location = 1) in vec4 a_Normal;
layout (location = 2) in vec4 a_Tangent;
layout (location = 3) in vec4 a_Bitangent;
layout (location = 4) in vec4 a_TexCoord;

out VS_OUT {
    vec3 Normal;
} vs_out;

void main() {
    mat4 model = b_Transforms.transforms[gl_InstanceID];
    vs_out.Normal = normalize(mat3(transpose(inverse(u_CameraData.view * model))) * a_Normal.xyz);
    gl_Position = u_CameraData.view * model * a_Pos;
}
