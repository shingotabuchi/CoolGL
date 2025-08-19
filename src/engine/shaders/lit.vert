#version 410 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 uMVP;

out vec3 vNormal; // world-space normal
out vec2 vUV;

void main()
{
    gl_Position = uMVP * vec4(aPos, 1.0);
    vNormal = uMVP * vec4(aNormal, 1.0);
    vUV = aUV;
}


