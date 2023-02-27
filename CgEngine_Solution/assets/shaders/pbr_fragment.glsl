#version 450 core

layout (binding = 0, std140) uniform CameraData {
    mat4 viewProjection;
    mat4 projection;
    mat4 view;
    vec3 position;
} u_CameraData;

layout (binding = 1, std140) uniform LightData {
    vec3 dirLightDirection;
    float dirLightIntensity;
    vec3 dirLightColor;
} u_LightData;

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

const float PI = 3.141592;

float geometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float geometrySmith(float NdotV, float NdotL, float roughness) {
    float ggx2  = geometrySchlickGGX(NdotV, roughness);
    float ggx1  = geometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

float distributionGGX(vec3 N, vec3 H, float roughness) {
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;
    float NdotH  = max(dot(N, H), 0.0);

    float denom = (NdotH * NdotH) * (alphaSq - 1.0) + 1.0;
    return alphaSq / (PI * denom * denom);
}

vec3 fresnelSchlick(vec3 F0, float cosTheta) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 calcDirLight(vec3 F0, vec3 matAlbedo, float matMetalness, float matRoughness, vec3 N, vec3 V, float NdotV) {
    if (u_LightData.dirLightIntensity == 0.0f) {
        return vec3(0.0f);
    }

    vec3 L = u_LightData.dirLightDirection;
    vec3 Lradiance =  u_LightData.dirLightColor * u_LightData.dirLightIntensity;
    vec3 H = normalize(L + V);

    float NdotL = max(dot(N, L), 0.0f);

    vec3 F = fresnelSchlick(F0, max(dot(H, V), 0.0f));
    float D = distributionGGX(N, H, matRoughness);
    float G = geometrySmith(NdotL, NdotV, matRoughness);

    vec3 kd = (1.0f - F) * (1.0f - matMetalness);
    vec3 diffuseBRDF = kd * matAlbedo;
    vec3 specularBRDF = (F * D * G) / max(0.0001, 4.0 * NdotL * NdotV);
    specularBRDF = clamp(specularBRDF, vec3(0.0f), vec3(10.0f));
    return (diffuseBRDF + specularBRDF) * Lradiance * NdotL;
}

void main()
{
    vec3 mat_AlbedoColor = texture(u_Mat_AlbedoTexture, fs_in.TexCoord).rgb * u_Mat_AlbedoColor;
    float mat_Metalness = texture(u_Mat_MetalnessTexture, fs_in.TexCoord).r * u_Mat_Metalness;
    float mat_Roughness = texture(u_Mat_RoughnessTexture, fs_in.TexCoord).r * u_Mat_Roughness;

    vec3 mat_Normal = normalize(fs_in.Normal);
    if (u_Mat_UseNormals) {
        mat_Normal = (texture(u_Mat_NormalTexture, fs_in.TexCoord).rgb * 2.0f - 1.0f);
    }

    vec3 V = normalize(u_CameraData.position - fs_in.WorldPosition);
    float NdotV = max(dot(mat_Normal, V), 0.0f);

    vec3 F0 = vec3(0.04f);
    F0 = mix(F0, mat_AlbedoColor, mat_Metalness);

    vec3 light = calcDirLight(F0, mat_AlbedoColor, mat_Metalness, mat_Roughness, mat_Normal, V, NdotV);
    light += mat_AlbedoColor * u_Mat_Emission;

    o_FragColor = vec4(light, 1.0f);
}
