#version 450 core

in VS_OUT {
    vec3 WorldPosition;
    vec3 Normal;
    vec2 TexCoord;
} fs_in;

out vec4 o_FragColor;

uniform vec3 u_Mat_AlbedoColor;
uniform float u_Mat_Metalness;
uniform float u_Mat_Roughness;
uniform float u_Mat_Emission;
uniform bool u_Mat_UseNormals;
layout(binding = 0) uniform sampler2D u_Mat_AlbedoTexture;
layout(binding = 1) uniform sampler2D u_Mat_NormalTexture;
layout(binding = 2) uniform sampler2D u_Mat_MetalnessTexture;
layout(binding = 3) uniform sampler2D u_Mat_RoughnessTexture;

void main()
{
    vec3 mat_AlbedoColor = texture(u_Mat_AlbedoTexture, fs_in.TexCoord).rgb * u_Mat_AlbedoColor;
    float mat_Metalness = texture(u_Mat_MetalnessTexture, fs_in.TexCoord).r * u_Mat_Metalness;
    float mat_Roughness = texture(u_Mat_RoughnessTexture, fs_in.TexCoord).r * u_Mat_Roughness;

    vec3 mat_Normals = normalize(fs_in.Normal);
    if (u_Mat_UseNormals) {
        mat_Normals = (texture(u_Mat_NormalTexture, fs_in.TexCoord).rgb * 2.0f - 1.0f);
    }

    o_FragColor = vec4(mat_AlbedoColor.rgb + mat_AlbedoColor.rgb * u_Mat_Emission, 1.0f);
}
