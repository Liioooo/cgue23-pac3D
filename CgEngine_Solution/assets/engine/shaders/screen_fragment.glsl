#version 450 core

in VS_OUT {
    vec2 TexCoord;
} fs_in;

layout (binding = 0, std140) uniform CameraData {
    mat4 viewProjection;
    mat4 projection;
    mat4 view;
    vec3 position;
    float exposure;
    float bloomIntensity;
    float bloomThreshold;
} u_CameraData;

layout (binding = 0) uniform sampler2D u_FinalImage;
layout (binding = 1) uniform sampler2D u_BloomTexture;

out vec4 o_FragColor;

vec3 gammaCorrect(vec3 color, float gamma) {
    return pow(color, vec3(1.0f / gamma));
}

vec3 toneMap(vec3 color, float exposure) {
    return vec3(1.0) - exp(-color * exposure);
}

vec3 upsampleBloom(sampler2D tex, vec2 texCoord) {
    vec2 texSize = 1.0f / vec2(textureSize(tex, 0));
    float x = texSize.x;
    float y = texSize.y;

    // a - b - c
    // d - e - f
    // g - h - i

    vec3 a = texture(tex, vec2(texCoord.x - x, texCoord.y + y)).rgb;
    vec3 b = texture(tex, vec2(texCoord.x,     texCoord.y + y)).rgb;
    vec3 c = texture(tex, vec2(texCoord.x + x, texCoord.y + y)).rgb;

    vec3 d = texture(tex, vec2(texCoord.x - x, texCoord.y)).rgb;
    vec3 e = texture(tex, vec2(texCoord.x,     texCoord.y)).rgb;
    vec3 f = texture(tex, vec2(texCoord.x + x, texCoord.y)).rgb;

    vec3 g = texture(tex, vec2(texCoord.x - x, texCoord.y - y)).rgb;
    vec3 h = texture(tex, vec2(texCoord.x,     texCoord.y - y)).rgb;
    vec3 i = texture(tex, vec2(texCoord.x + x, texCoord.y - y)).rgb;

    vec3 result = e * 4.0f;
    result += (b+d+f+h) * 2.0f;
    result += (a+c+g+i);
    result *= 1.0f / 16.0f;

    return result;
}

void main() {
    vec4 hdrColor = texture(u_FinalImage, fs_in.TexCoord);

    vec3 bloom = upsampleBloom(u_BloomTexture, fs_in.TexCoord) * u_CameraData.bloomIntensity;

    hdrColor += vec4(bloom, 1.0f);
    hdrColor *= u_CameraData.exposure;

    vec3 fragColor = toneMap(hdrColor.rgb, 1);
    fragColor = gammaCorrect(fragColor, 2.2f);
    o_FragColor = vec4(fragColor, 1.0f);
}
