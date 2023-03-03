#version 450 core

in VS_OUT {
    vec2 TexCoord;
} fs_in;

layout (binding = 0) uniform sampler2D u_FinalImage;

out vec4 o_FragColor;

vec3 gammaCorrect(vec3 color, float gamma) {
    return pow(color, vec3(1.0f / gamma));
}

vec3 toneMap(vec3 color, float exposure) {
    return vec3(1.0) - exp(-color * exposure);
}

void main()
{
    vec4 hdrColor = texture(u_FinalImage, fs_in.TexCoord);
    vec3 fragColor = toneMap(hdrColor.rgb, 1);
    fragColor = gammaCorrect(fragColor, 2.2f);
    o_FragColor = vec4(fragColor, 1.0f);
}
