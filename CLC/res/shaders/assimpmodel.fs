#version 460 core
struct Light {
    int type;
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
};



struct Material {
    sampler2D diffuse1;
    // Add more textures if needed
};

#define MAX_LIGHTS 128
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform int numLights;
uniform bool useLighting;
uniform Light lights[MAX_LIGHTS];
uniform vec3 objectColor;
uniform bool useTexture;

uniform Material material;  // Changed from individual sampler2D to Material struct

uniform sampler2D shadowMap;
uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lights[0].position - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    if(projCoords.z > 1.0) shadow = 0.0;
    return shadow;
}

void main() {
    vec3 baseColor = objectColor;
    if (useTexture) {
        baseColor = texture(material.diffuse1, TexCoords).rgb;
    }

    if (!useLighting || numLights == 0) {
        FragColor = vec4(baseColor, 1.0);
        return;
    }

    vec3 norm = normalize(Normal);
    vec3 lighting = vec3(0.1) * baseColor; // Ambient
    
    for(int i = 0; i < min(numLights, MAX_LIGHTS); i++) {
        vec3 lightDir = normalize(lights[i].position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lights[i].color * lights[i].intensity;
        
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
        vec3 specular = spec * lights[i].color * lights[i].intensity;
        
        float shadow = (i == 0) ? ShadowCalculation(FragPosLightSpace) : 0.0;
        lighting += (1.0 - shadow) * (diffuse + specular) * baseColor;
    }
    
    FragColor = vec4(lighting, 1.0);
}