#version 460 core

out vec4 FragColor;

uniform sampler2D accumTexture;

void main() {
    vec2 uv = gl_FragCoord.xy / vec2(textureSize(accumTexture, 0));

    vec4 prevColor = texture(accumTexture, uv);
    vec3 gammaCorrected = pow(prevColor.xyz, vec3(1.0 / 2.2));

    FragColor = vec4(gammaCorrected, prevColor.a);
}