#version 450 core

#include "common/CameraDataBuffer.glsl"

layout (location = 0) in vec4 a_PosUV;
layout (location = 1) in vec4 a_LineColor;
layout (location = 2) in vec4 a_FillColor;
layout (location = 3) in float a_LineWidth;

out VS_OUT {
    vec2 TexCoord;
    vec4 LineColor;
    vec4 FillColor;
    float LineWidth;
} vs_out;

void main() {
    gl_Position = u_CameraData.uiProjectionMatrix * vec4(a_PosUV.x, a_PosUV.y, 0.0f, 1.0);
    vs_out.TexCoord = a_PosUV.zw;
    vs_out.LineColor = a_LineColor;
    vs_out.FillColor = a_FillColor;
    vs_out.LineWidth = a_LineWidth;
}
