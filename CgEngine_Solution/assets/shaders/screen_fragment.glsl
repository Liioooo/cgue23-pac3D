#version 450 core

in VS_OUT {
    vec2 TexCoord;
} fs_in;

layout (binding = 0) uniform sampler2D u_FinalImage;

out vec4 o_FragColor;

void main()
{
    vec4 fragColor = texture(u_FinalImage, fs_in.TexCoord);

    float gamma = 2.2;
    fragColor.rgb = pow(fragColor.rgb, vec3(1.0/gamma));
    o_FragColor = vec4(fragColor.rgb, 1.0f);
}
