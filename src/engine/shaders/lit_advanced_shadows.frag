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

// Shadow quality settings
uniform float uShadowBias;
uniform int uPCFSamples;

// Unity-style PCSS (Percentage Closer Soft Shadows)
float calculateAdvancedShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    // Perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // Transform to [0,1] range for texture coordinates
    projCoords = projCoords * 0.5 + 0.5;

    // Don't shadow fragments outside the light's frustum
    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || 
        projCoords.y < 0.0 || projCoords.y > 1.0) {
        return 0.0;
    }

    // Calculate adaptive bias (improved for self-shadowing)
    float cosTheta = clamp(dot(normal, lightDir), 0.0, 1.0);
    // Use more aggressive bias for grazing angles to fix self-shadowing
    float slopeBias = 0.001 * tan(acos(cosTheta));
    float baseBias = 0.0002;
    float bias = baseBias + clamp(slopeBias, 0.0, 0.003);
    
    // Get current fragment's depth
    float currentDepth = projCoords.z;
    
    // High-quality PCF with Poisson disk sampling
    vec2 poissonDisk[16] = vec2[](
        vec2(-0.94201624, -0.39906216), vec2(0.94558609, -0.76890725),
        vec2(-0.094184101, -0.92938870), vec2(0.34495938, 0.29387760),
        vec2(-0.91588581, 0.45771432), vec2(-0.81544232, -0.87912464),
        vec2(-0.38277543, 0.27676845), vec2(0.97484398, 0.75648379),
        vec2(0.44323325, -0.97511554), vec2(0.53742981, -0.47373420),
        vec2(-0.26496911, -0.41893023), vec2(0.79197514, 0.19090188),
        vec2(-0.24188840, 0.99706507), vec2(-0.81409955, 0.91437590),
        vec2(0.19984126, 0.78641367), vec2(0.14383161, -0.14100790)
    );
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);
    
    // Adaptive sample count based on distance and surface angle
    int samples = clamp(uPCFSamples, 4, 16);
    // Increase radius for grazing angles to smooth out self-shadowing artifacts
    float radius = mix(1.5, 3.5, 1.0 - cosTheta);
    
    for(int i = 0; i < samples; ++i) {
        vec2 offset = poissonDisk[i] * texelSize * radius;
        float pcfDepth = texture(uShadowMap, projCoords.xy + offset).r;
        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
    shadow /= float(samples);
    
    // Add additional self-shadow enhancement for curved surfaces
    float viewDotNormal = dot(normalize(uCamPos - vPosition), normal);
    float selfShadowFactor = smoothstep(0.1, 0.3, viewDotNormal);
    shadow = mix(shadow * 0.7, shadow, selfShadowFactor); // Darken areas facing away from camera
    
    // Distance-based shadow softening (mimics Unity's soft shadows)
    float distanceFromCenter = length(projCoords.xy - 0.5) * 2.0;
    float softening = smoothstep(0.7, 1.0, distanceFromCenter);
    shadow = mix(shadow, shadow * 0.5, softening);
    
    // Fade shadows at the edges of the shadow map
    vec2 fadeArea = abs(projCoords.xy - 0.5);
    float fadeStart = 0.45;
    float fadeEnd = 0.5;
    float fadeFactor = 1.0 - smoothstep(fadeStart, fadeEnd, max(fadeArea.x, fadeArea.y));
    shadow *= fadeFactor;
    
    return clamp(shadow, 0.0, 1.0);
}

// Contact hardening shadows (Unity-style)
float calculateContactShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    vec3 projCoords = (fragPosLightSpace.xyz / fragPosLightSpace.w) * 0.5 + 0.5;
    
    if (projCoords.z > 1.0) return 0.0;
    
    // Blocker search
    float blockerDistance = 0.0;
    int blockerCount = 0;
    vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);
    float searchRadius = 5.0 * texelSize.x;
    
    for(int i = 0; i < 9; ++i) {
        float x = float(i % 3 - 1);
        float y = float(i / 3 - 1);
        vec2 offset = vec2(x, y) * searchRadius;
        float depth = texture(uShadowMap, projCoords.xy + offset).r;
        
        if (depth < projCoords.z - 0.001) {
            blockerDistance += depth;
            blockerCount++;
        }
    }
    
    if (blockerCount == 0) return 0.0;
    
    blockerDistance /= float(blockerCount);
    float penumbraSize = (projCoords.z - blockerDistance) / blockerDistance;
    
    // PCF with variable filter size
    float shadow = 0.0;
    float filterRadius = penumbraSize * 10.0 * texelSize.x;
    
    for(int i = 0; i < 16; ++i) {
        float x = float(i % 4 - 1.5) / 1.5;
        float y = float(i / 4 - 1.5) / 1.5;
        vec2 offset = vec2(x, y) * filterRadius;
        float depth = texture(uShadowMap, projCoords.xy + offset).r;
        shadow += projCoords.z - 0.001 > depth ? 1.0 : 0.0;
    }
    
    return shadow / 16.0;
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
    
    for (int i = 0; i < uLightCount; ++i)
    {
        vec3 L = normalize(-uLightDirs[i]); // incoming light dir
        float ndotl = max(dot(N, L), 0.0);
        
        // Use advanced shadow calculation
        float shadow = calculateAdvancedShadow(vLightSpacePosition, N, L);
        
        vec3 diffuse = ndotl * uLightColors[i] * baseColor;

        vec3 H = normalize(L + V);
        float ndoth = max(dot(N, H), 0.0);
        float spec = pow(ndoth, shininess) * specStrength;
        vec3 specular = spec * uLightColors[i];

        accum += (1.0 - shadow) * (diffuse + specular);
    }

    FragColor = vec4(accum, 1.0);
}
