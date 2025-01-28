#version 330 core

out vec4 FragColor;

in vec3 pos;

uniform float time;

float rand(vec2 co)
{
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
    float random = rand(pos.xy * time);

    FragColor = vec4(random, random, random, 1.0);
}