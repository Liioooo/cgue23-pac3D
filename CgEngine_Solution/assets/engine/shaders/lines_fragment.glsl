#version 450 core

out vec4 o_FragColor;

in VS_OUT {
    vec3 Color;
} fs_in;

void main() {
    o_FragColor = vec4(fs_in.Color, 1.0f);
}