#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

out vec3 pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;

void main() {
    pos = aPosition; 
    gl_Position = perspective * view * model * vec4(aPosition, 1.0);
}