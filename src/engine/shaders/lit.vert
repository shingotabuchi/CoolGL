#version 410 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

layout(location = 3) in mat4 aInstanceMatrix;

uniform mat4 uProjection; 
uniform mat4 uView;
uniform mat4 uModel;
uniform bool u_isInstanced;

out vec3 vPosition; // world-space position
out vec3 vNormal;   // world-space normal
out vec2 vUV;

void main()
{
    mat4 finalModelMatrix;
    if (u_isInstanced)
    {
        finalModelMatrix = aInstanceMatrix;
    }
    else
    {
        finalModelMatrix = uModel;
    }

    gl_Position = uProjection * uView * finalModelMatrix * vec4(aPos, 1.0);

    vPosition = vec3(finalModelMatrix * vec4(aPos, 1.0));

    vNormal = transpose(inverse(mat3(finalModelMatrix))) * aNormal;

    vUV = aUV;
}