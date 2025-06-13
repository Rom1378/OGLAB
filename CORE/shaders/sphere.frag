#version 460 core

struct Light {
    int type;       // 0: Point, 1: Directional, 2: Spot
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
};

#define MAX_LIGHTS 128
#define POINT_LIGHT 0
#define DIRECTIONAL_LIGHT 1
#define SPOT_LIGHT 2

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform int numLights;
uniform bool useLighting;
uniform Light lights[MAX_LIGHTS];
uniform vec3 objectColor;
uniform vec3 viewPos;  // Camera position for specular calculations

void main() {
    vec3 resultColor = objectColor * 0.1; // Ambient base light

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

    FragColor = vec4(resultColor, 1.0);
}
