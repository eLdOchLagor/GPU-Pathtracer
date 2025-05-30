#version 460 core

layout(location = 0) in vec3 aPosition;

out vec3 pos;

void main() {
    pos = aPosition; 
    gl_Position = vec4(aPosition, 1.0);
}