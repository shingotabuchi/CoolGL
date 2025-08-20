#version 410 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

layout(location = 3) in mat4 aInstanceMatrix;

uniform float u_time;
uniform vec3 u_wave_params;
uniform mat4 uProjection; 
uniform mat4 uView;
uniform mat4 uModel;
uniform bool u_isInstanced;
uniform mat4 uLightSpaceMatrix;

out vec3 vPosition; // world-space position
out vec3 vNormal;   // world-space normal
out vec4 vLightSpacePosition;
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

    vec3 world_position = vec3(finalModelMatrix * vec4(aPos, 1.0));

    vec3 animated_position = aPos;
    animated_position.z += (1.0 + sin(u_time * u_wave_params.z + gl_InstanceID * u_wave_params.y)) * u_wave_params.x;
    
    gl_Position = uProjection * uView * finalModelMatrix * vec4(animated_position, 1.0);

    vPosition = vec3(finalModelMatrix * vec4(animated_position, 1.0));
    vNormal = transpose(inverse(mat3(finalModelMatrix))) * aNormal;
    vLightSpacePosition = uLightSpaceMatrix * vec4(vPosition, 1.0);

    vUV = aUV;
}