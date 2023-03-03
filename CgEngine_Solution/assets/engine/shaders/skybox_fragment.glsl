#version 450 core

in VS_OUT {
    vec3 TexCoord;
} fs_in;

layout (binding = 0) uniform samplerCube u_Texture;

uniform float u_Lod;
uniform float u_Intensity;

out vec4 o_FragColor;

void main() {
    o_FragColor = textureLod(u_Texture, fs_in.TexCoord, u_Lod) * u_Intensity;
    o_FragColor.a = 1.0f;
}
