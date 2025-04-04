#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aCenter;
layout (location = 2) in vec4 aScale;

out int depth;

uniform mat4 pv;

void main() {
    vec3 worldPos = aCenter.xyz + aPos * aScale.xyz;
    gl_Position = pv * vec4(worldPos, 1.0);
    depth = int(aScale.w);
}