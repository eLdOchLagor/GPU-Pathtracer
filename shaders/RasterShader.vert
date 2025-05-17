#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

out vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 perspective;

void main() {
    normal = mat3(perspective) * mat3(view) * mat3(model) * aNormal;

    gl_Position = perspective * view * model * vec4(aPosition, 1.0);
}