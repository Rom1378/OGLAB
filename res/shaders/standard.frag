#version 460 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    float shininess;
}; 

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

#define MAX_DIR_LIGHTS 4
#define MAX_POINT_LIGHTS 8
#define MAX_SPOT_LIGHTS 4
#define MAX_SHADOW_MAPS 8

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;

uniform vec3 viewPos;
uniform Material material;

// Texture presence flags
uniform bool hasDiffuseMap;
uniform bool hasSpecularMap;
uniform bool hasNormalMap;

uniform DirLight dirLights[MAX_DIR_LIGHTS];
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

uniform int numDirLights;
uniform int numPointLights;
uniform int numSpotLights;

uniform vec3 objectColor;
uniform bool useTexture;
uniform bool useLighting;
uniform bool useShadow;
uniform bool blinn;

// Shadow maps
uniform sampler2D shadowMaps2D[MAX_SHADOW_MAPS];
uniform samplerCube shadowMapsCube[MAX_SHADOW_MAPS];
uniform mat4 lightSpaceMatrices[MAX_SHADOW_MAPS];
uniform vec3 shadowLightPositions[MAX_SHADOW_MAPS];
uniform float shadowFarPlanes[MAX_SHADOW_MAPS];
uniform int num2DShadowMaps;
uniform int numCubeShadowMaps;

// Light to shadow map index mapping
uniform int dirLightShadowIndices[MAX_DIR_LIGHTS];
uniform int pointLightShadowIndices[MAX_POINT_LIGHTS];
uniform int spotLightShadowIndices[MAX_SPOT_LIGHTS];

// Function declarations
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, int lightIndex);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, int lightIndex);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, int lightIndex);
float ShadowCalculationCube(int cubeMapIndex, int lightPosIndex, vec3 fragPos);
float ShadowCalculation2D(int mapIndex, int lightPosIndex, vec3 fragPos);

// Helper function to get diffuse color
vec3 GetDiffuseColor() {
    if (hasDiffuseMap) {
        return texture(material.diffuse, TexCoords).rgb;
    }
    return objectColor;
}

// Helper function to get specular color
vec3 GetSpecularColor() {
    if (hasSpecularMap) {
        return texture(material.specular, TexCoords).rgb;
    }
    return vec3(0.5); // Default specular intensity
}

// Helper function to get normal
vec3 GetNormal() {
    if (hasNormalMap) {
        // Sample normal map in range [0,1]
        vec3 normal = texture(material.normal, TexCoords).rgb;
        // Transform to range [-1,1]
        normal = normal * 2.0 - 1.0;
        // Transform to world space using TBN matrix
        return normalize(TBN * normal);
    }
    return normalize(Normal);
}

void main() {
    vec3 norm = GetNormal();
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 result = vec3(0.0);
    
    if (useLighting) {
        // Calculate lighting from all light sources
        for (int i = 0; i < numDirLights; i++) {
            result += CalcDirLight(dirLights[i], norm, viewDir, i);
        }
        
        for (int i = 0; i < numPointLights; i++) {
            result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, i);
        }
        
        for (int i = 0; i < numSpotLights; i++) {
            result += CalcSpotLight(spotLights[i], norm, FragPos, viewDir, i);
        }
    }
    else if (useTexture) {
        // No lighting, just show texture or object color
        result = GetDiffuseColor();
    }
    else {
        // No texture, no lighting
        result = objectColor;
    }

    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, int lightIndex)
{
    vec3 lightDir = normalize(-light.direction);
    float NdotL = dot(normal, lightDir);

    // Ambient (always present)
    vec3 ambient = light.ambient * GetDiffuseColor();

    // Early exit if surface faces away from light
    if (NdotL <= 0.0) {
        return ambient;
    }

    // Diffuse
    float diff = NdotL;
    vec3 diffuse = light.diffuse * diff * GetDiffuseColor();
    
    // Specular
    float spec = 0.0;
    if (blinn) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    } else {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }
    vec3 specular = light.specular * spec * GetSpecularColor();
    
    // Shadow calculation
    float shadow = 0.0;
    if (useShadow && dirLightShadowIndices[lightIndex] >= 0) {
        shadow = ShadowCalculation2D(dirLightShadowIndices[lightIndex], lightIndex, FragPos);
    }
    
    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, int lightIndex)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    // Ambient
    vec3 ambient = light.ambient * GetDiffuseColor() * attenuation;
    
    float NdotL = dot(normal, lightDir);

    // Early exit for back-facing surfaces (reduced ambient only)
    if (NdotL <= 0.0) {
        return ambient * 0.1; // Very dim ambient for back faces
    }
    
    // Diffuse
    float diff = NdotL;
    vec3 diffuse = light.diffuse * diff * GetDiffuseColor() * attenuation;
    
    // Specular
    float spec = 0.0;
    if (blinn) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    } else {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }
    vec3 specular = light.specular * spec * GetSpecularColor() * attenuation;
    
    // Shadow calculation
    float shadow = 0.0;
    if (useShadow && pointLightShadowIndices[lightIndex] >= 0) {
        int shadowInfoIndex = numDirLights + lightIndex;
        shadow = ShadowCalculationCube(pointLightShadowIndices[lightIndex], shadowInfoIndex, fragPos);
    }
    
    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, int lightIndex)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    // Spotlight intensity (cone)
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    // Ambient
    vec3 ambient = light.ambient * GetDiffuseColor() * attenuation * intensity;
    
    float NdotL = dot(normal, lightDir);

    // Early exit for back-facing surfaces
    if (NdotL <= 0.0) {
        return ambient * 0.1;
    }

    // Diffuse
    float diff = NdotL;
    vec3 diffuse = light.diffuse * diff * GetDiffuseColor() * attenuation * intensity;
    
    // Specular
    float spec = 0.0;
    if (blinn) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    } else {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }
    vec3 specular = light.specular * spec * GetSpecularColor() * attenuation * intensity;
    
    // Shadow calculation
    float shadow = 0.0;
    if (useShadow && spotLightShadowIndices[lightIndex] >= 0) {
        int shadowInfoIndex = numDirLights + numPointLights + lightIndex;
        shadow = ShadowCalculation2D(spotLightShadowIndices[lightIndex], shadowInfoIndex, fragPos);
    }
    
    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

float ShadowCalculationCube(int cubeMapIndex, int lightPosIndex, vec3 fragPos)
{
    if (cubeMapIndex < 0 || cubeMapIndex >= numCubeShadowMaps) {
        return 0.0;
    }
    
    vec3 fragToLight = fragPos - shadowLightPositions[lightPosIndex];
    float closestDepth = texture(shadowMapsCube[cubeMapIndex], fragToLight).r;
    closestDepth *= shadowFarPlanes[cubeMapIndex];
    float currentDepth = length(fragToLight);
    float bias = 0.05;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    
    return shadow;
}

float ShadowCalculation2D(int mapIndex, int lightPosIndex, vec3 fragPos)
{
    if (mapIndex < 0 || mapIndex >= num2DShadowMaps) {
        return 0.0;
    }
    
    vec4 fragPosLightSpace = lightSpaceMatrices[mapIndex] * vec4(fragPos, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if (projCoords.z > 1.0) {
        return 0.0;
    }
    
    float closestDepth = texture(shadowMaps2D[mapIndex], projCoords.xy).r; 
    float currentDepth = projCoords.z;

    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(shadowLightPositions[lightPosIndex] - fragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMaps2D[mapIndex], 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMaps2D[mapIndex], projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    return shadow;
}