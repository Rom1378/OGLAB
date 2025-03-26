#version 460 core

struct Light {
    int type;   // 0 = point, 1 = directional, 2 = spot
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
};

#define MAX_LIGHTS 128
#define POINT_LIGHT 0
#define DIRECTIONAL_LIGHT 1
#define SPOT_LIGHT 2

in vec3 FragPos;  // Fragment position in world space
in vec3 Normal;   // Transformed normal
in vec2 TexCoords;  // Texture coordinates

out vec4 FragColor;

uniform int numLights;
uniform bool useLighting;
uniform Light lights[MAX_LIGHTS];

uniform vec3 objectColor; // Object color
uniform sampler2D texture1;
uniform bool useTexture; // Texture usage flag

uniform vec3 viewPos;  // Camera position for specular calculations


void main() {
    // Base ambient light
    vec3 resultColor = objectColor * 0.1; 

    // Lighting calculations
    if (useLighting) {
        vec3 norm = normalize(Normal);
        
        for (int i = 0; i < numLights; i++) {
            vec3 lightContribution = vec3(0.0);
            
            if (lights[i].type == POINT_LIGHT) {
                // Point Light Calculation
                vec3 lightDir = normalize(lights[i].position - FragPos);
                float diff = max(dot(norm, lightDir), 0.0);
                lightContribution = diff * lights[i].color * lights[i].intensity;
            }
            else if (lights[i].type == DIRECTIONAL_LIGHT) {
                // Directional Light Calculation
                vec3 lightDir = normalize(-lights[i].direction);
                float diff = max(dot(norm, lightDir), 0.0);
                lightContribution = diff * lights[i].color * lights[i].intensity;
            }
            else if (lights[i].type == SPOT_LIGHT) {
                // Spot Light Calculation (basic implementation)
                vec3 lightDir = normalize(lights[i].position - FragPos);
                float diff = max(dot(norm, lightDir), 0.0);
                
                // Add spot light cone effect
                float theta = dot(lightDir, normalize(-lights[i].direction));
                float epsilon = 0.1; // Inner cutoff
                float intensity = clamp((theta - (1.0 - epsilon)) / epsilon, 0.0, 1.0);
                
                lightContribution = diff * lights[i].color * lights[i].intensity * intensity;
            }
            
            resultColor += lightContribution * objectColor;
        }

    }

    // Apply texture if enabled
    vec4 finalColor = vec4(resultColor, 1.0);
    if (useTexture) {
        vec4 texColor = texture(texture1, TexCoords);
        finalColor.rgb *= texColor.rgb;
        finalColor.a *= texColor.a; // Maintain alpha blending
    }

    FragColor = finalColor;
}