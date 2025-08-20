#version 410 core

const int MAX_LIGHTS = 4;

in vec3 vPosition; // world-space
in vec3 vNormal; // world-space
in vec2 vUV;
in vec4 vLightSpacePosition;

out vec4 FragColor;

// Light uniforms (world-space incoming light directions)
uniform sampler2D uShadowMap;
uniform int uLightCount;
uniform vec3 uLightDirs[MAX_LIGHTS];
uniform vec3 uLightColors[MAX_LIGHTS];
uniform vec3 uAmbient;

// Material
uniform sampler2D uAlbedo;
uniform int uUseTexture;
uniform vec3 uColor;       // tint
uniform float uSmoothness; // [0..1]
uniform vec3 uCamPos;     // world-space cam position

float calculateShadow(vec4 fragPosLightSpace) {
    // Perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // Transform to [0,1] range for texture coordinates
    projCoords = projCoords * 0.5 + 0.5;

    // Don't shadow fragments outside the light's frustum
    if (projCoords.z > 1.0) {
        return 0.0;
    }

    // Get closest depth from shadow map
    float closestDepth = texture(uShadowMap, projCoords.xy).r;
    
    // Get current fragment's depth
    float currentDepth = projCoords.z;
    
    // Add a small bias to prevent shadow acne
    float bias = 0.0001;

    // Check if in shadow
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    
    return shadow;
}

void main()
{
    vec3 N = normalize(vNormal);
    vec3 texColor = (uUseTexture != 0) ? texture(uAlbedo, vUV).rgb : vec3(1.0);
    vec3 baseColor = texColor * uColor;

    // Diffuse + Specular (Blinn-Phong)
    vec3 V = normalize(uCamPos - vPosition);
    float shininess = mix(1.0, 256.0, clamp(uSmoothness, 0.0, 1.0));
    float specStrength = 0.5;

    vec3 accum = uAmbient * baseColor;
    float shadow = calculateShadow(vLightSpacePosition);
    for (int i = 0; i < uLightCount; ++i)
    {
        vec3 L = normalize(-uLightDirs[i]); // incoming light dir
        float ndotl = max(dot(N, L), 0.0);
        vec3 diffuse = ndotl * uLightColors[i] * baseColor;

        vec3 H = normalize(L + V);
        float ndoth = max(dot(N, H), 0.0);
        float spec = pow(ndoth, shininess) * specStrength;
        vec3 specular = spec * uLightColors[i];

        accum += (1.0 - shadow) * (diffuse + specular);
    }

    FragColor = vec4(accum, 1.0);
}


