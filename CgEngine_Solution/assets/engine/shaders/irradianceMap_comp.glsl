#version 450 core

layout(binding = 0) uniform samplerCube u_cubeMap;
layout(binding = 1, rgba32f) restrict writeonly uniform imageCube o_IrradianceMap;

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

const float PI = 3.14159265359f;
const float TWO_PI = PI * 2.0f;
const float HALF_PI = PI * 0.5f;

const float sampleDelta = 0.025f;
const float totalSamples = (TWO_PI / sampleDelta) * (HALF_PI / sampleDelta);
const float invTotalSamples = 1.0 / totalSamples;

vec3 getSampleDirection() {
    vec2 st = gl_GlobalInvocationID.xy / vec2(imageSize(o_IrradianceMap));
    vec2 uv = 2.0 * vec2(st.x, 1.0 - st.y) - vec2(1.0);

    vec3 direction;
    if (gl_GlobalInvocationID.z == 0)      direction = vec3(  1.0, uv.y, -uv.x);
    else if (gl_GlobalInvocationID.z == 1) direction = vec3( -1.0, uv.y,  uv.x);
    else if (gl_GlobalInvocationID.z == 2) direction = vec3( uv.x,  1.0, -uv.y);
    else if (gl_GlobalInvocationID.z == 3) direction = vec3( uv.x, -1.0,  uv.y);
    else if (gl_GlobalInvocationID.z == 4) direction = vec3( uv.x, uv.y,   1.0);
    else if (gl_GlobalInvocationID.z == 5) direction = vec3(-uv.x, uv.y,  -1.0);
    return normalize(direction);
}

void main() {
    vec3 N = getSampleDirection();

    vec3 irradiance = vec3(0.0);

    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, N));
    up = normalize(cross(N, right));

    for (float phi = 0.0f; phi < TWO_PI; phi += sampleDelta) {
        for (float theta = 0.0f; theta < HALF_PI; theta += sampleDelta) {
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

            irradiance += texture(u_cubeMap, sampleVec).rgb * cos(theta) * sin(theta);
        }
    }
    irradiance = PI * irradiance * invTotalSamples;

    imageStore(o_IrradianceMap, ivec3(gl_GlobalInvocationID), vec4(irradiance, 1.0));
}
