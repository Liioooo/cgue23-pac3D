#version 450 core

in VS_OUT {
    vec2 TexCoord;
    vec4 LineColor;
    vec4 FillColor;
    float LineWidth;
} fs_in;

out vec4 o_FragColor;

void main() {
    vec2 localPos = (fs_in.TexCoord - 0.5f) * 2.0f;
    float dist = length(localPos);

    if (dist > 1.0f) {
        discard;
    }

    float lineAlpha = 1.0f - smoothstep(1.0f - fs_in.LineWidth, 1.02f - fs_in.LineWidth, dist);
    float fillAlpha = 1.0f - smoothstep(0.95f, 1.0f, dist);

    o_FragColor = vec4(mix(fs_in.LineColor.rgb, fs_in.FillColor.rgb, lineAlpha), mix(fs_in.LineColor.a, fs_in.FillColor.a, lineAlpha) * fillAlpha);

}
