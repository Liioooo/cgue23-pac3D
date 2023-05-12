layout (binding = 0, std140) uniform CameraData {
    mat4 viewProjection;
    mat4 projection;
    mat4 view;
    mat4 uiProjectionMatrix;
    vec3 position;
    float exposure;
    float bloomIntensity;
    float bloomThreshold;
} u_CameraData;