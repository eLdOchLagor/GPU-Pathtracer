#version 330 core

out vec4 FragColor;

in vec3 pos;

uniform float time;

float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))
                 * 43758.5453123);
}

// 2D Noise based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float noise(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f*f*(3.0-2.0*f);
    // u = smoothstep(0.,1.,f);

    // Mix 4 coorners percentages
    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

float multiOctave(vec2 st){
    st.x += cos(time) * 10.0f;
    st.y += sin(time) * 10.0f;

    float frequency = 0.5f;
    float amplitude = 1.0f;

    float totalNoise = 0.0f;

    for(int i = 0; i < 5; i++){
        frequency *= 2.0f;
        amplitude *= 0.5f;
        totalNoise += noise(st*frequency)*amplitude;
    }

    return totalNoise;
}

void main() {
    float perlin = multiOctave(pos.xy*10.0f);

    FragColor = vec4(perlin, perlin, perlin, 1.0);
}