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
in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform int numLights;
uniform bool useLighting;
uniform Light lights[MAX_LIGHTS];
uniform vec3 objectColor;
uniform sampler2D texture1;
uniform bool useTexture;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform mat4 lightSpaceMatrix;  
uniform vec3 viewPos;
uniform vec3 lightPos;  

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005) * 0.1;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main() {
    // Determine the base color
    vec3 baseColor = objectColor; // Start with object color
    
    // If using texture, replace base color with texture color
    if (useTexture) {
        baseColor = texture(texture1, TexCoords).rgb;
    }
    
    // Get diffuse texture contribution (assuming it's a multiplier)
    vec3 diffuseTex = texture(diffuseTexture, TexCoords).rgb;
    
    vec3 norm = normalize(Normal);
    vec3 lightColor = lights[0].color;

    // Lighting calculations
    vec3 lightDir = normalize(lights[0].position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 ambient = 0.3 * lightColor;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    
    // Calculate shadow
    float shadow = ShadowCalculation(FragPosLightSpace);                      
    
    // Combine all components
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * baseColor * diffuseTex;
    
    // Final color output
    FragColor = vec4(lighting, 1.0);
}