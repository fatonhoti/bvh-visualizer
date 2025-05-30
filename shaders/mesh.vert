#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 mvp;

out vec3 FragPos;
out vec3 Normal;

void main() {
    gl_Position = mvp * vec4(aPos, 1.0);
    FragPos = vec3(vec4(aPos, 1.0));
    Normal = aNormal;
}