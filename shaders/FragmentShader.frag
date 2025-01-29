#version 330 core

out vec4 FragColor;

uniform float time;

uniform vec3 cameraPosition;
uniform vec3 forward;
uniform vec3 right;
uniform vec3 up;
uniform float imagePlaneWidth;
uniform float imagePlaneHeight;
uniform int screenWidth;
uniform int screenHeight;

in vec3 pos;

void main() {
   
    vec4 coord = gl_FragCoord;

    // Coordinates in imagePlane
    float u = coord.x / screenWidth * imagePlaneWidth - imagePlaneWidth/2.0;
    float v = (1.0 - coord.y / screenHeight) * imagePlaneHeight - imagePlaneHeight/2.0;

    FragColor = vec4(u, v, 0, 1);
}