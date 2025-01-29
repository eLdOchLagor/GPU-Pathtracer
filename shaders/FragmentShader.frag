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
    st.x += time * 10.0f;
    

    //st.xy = vec2(cos(time) + sin(time), cos(time) - sin(time);
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
    //perlin = clamp(perlin, 0.25, 1);
    FragColor = vec4(220.0/255.0, 220.0/255.0,220.0/255.0,1.0);

    if(perlin < 0.72){
    FragColor = vec4(130.0/255.0, 130.0/255.0,130.0/255.0,1.0);

    }

    if(perlin < 0.64){
        FragColor = vec4(123.0/255.0, 187.0/255.0,160.0/255.0,1.0);
    }
    if(perlin < 0.52){
        
        FragColor = vec4(222.0/255.0,205.0/255.0,180.0/255.0,1.0);
    }
    if(perlin < 0.45){
        FragColor = vec4(71.0/255.0, 164.0/255.0, 204.0/255.0, 1.0);
    }
}