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

// Enhanced shadow calculation with aggressive self-shadowing for neck areas
float calculateEnhancedShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    // Perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // Transform to [0,1] range for texture coordinates
    projCoords = projCoords * 0.5 + 0.5;

    // Don't shadow fragments outside the light's frustum
    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || 
        projCoords.y < 0.0 || projCoords.y > 1.0) {
        return 0.0;
    }

    // More aggressive bias for better self-shadowing
    float cosTheta = clamp(dot(normal, lightDir), 0.0, 1.0);
    float bias = 0.0001 + 0.001 * sqrt(1.0 - cosTheta * cosTheta); // Aggressive bias for curved areas
    
    // Get current fragment's depth
    float currentDepth = projCoords.z;
    
    // Simplified but effective PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);
    
    // Large radius for better self-shadowing detection
    float radius = 2.5;
    int samples = 9;
    
    // 3x3 PCF kernel
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            vec2 offset = vec2(x, y) * texelSize * radius;
            float pcfDepth = texture(uShadowMap, projCoords.xy + offset).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    
    // AGGRESSIVE neck shadow enhancement
    vec3 viewDir = normalize(uCamPos - vPosition);
    float viewDotNormal = dot(viewDir, normal);
    float lightDotNormal = dot(lightDir, normal);
    
    // Detect neck-like areas (surfaces facing away from both light and camera)
    float neckFactor = (1.0 - viewDotNormal) * (1.0 - lightDotNormal);
    neckFactor = smoothstep(0.2, 0.8, neckFactor);
    
    // Force shadows in neck areas
    float forcedNeckShadow = mix(0.0, 0.7, neckFactor);
    shadow = max(shadow, forcedNeckShadow);
    
    // Additional ambient occlusion-like effect for undersides
    float undersideFactor = smoothstep(-0.2, 0.2, lightDotNormal);
    shadow = mix(shadow * 1.5, shadow, undersideFactor);
    
    // Edge fade
    vec2 fadeArea = abs(projCoords.xy - 0.5);
    float fadeFactor = 1.0 - smoothstep(0.45, 0.5, max(fadeArea.x, fadeArea.y));
    shadow *= fadeFactor;
    
    return clamp(shadow, 0.0, 0.85); // Cap shadow to prevent complete blackness
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
        
        // Use enhanced shadow calculation
        float shadow = calculateEnhancedShadow(vLightSpacePosition, N, L);
        
        vec3 diffuse = ndotl * uLightColors[i] * baseColor;

        vec3 H = normalize(L + V);
        float ndoth = max(dot(N, H), 0.0);
        float spec = pow(ndoth, shininess) * specStrength;
        vec3 specular = spec * uLightColors[i];

        accum += (1.0 - shadow) * (diffuse + specular);
    }

    FragColor = vec4(accum, 1.0);
}
