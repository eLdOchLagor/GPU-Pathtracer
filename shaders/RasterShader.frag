#version 460 core

in vec3 pos;
out vec4 FragColor;

void main() {
    FragColor = vec4(pos, 1.0);
}