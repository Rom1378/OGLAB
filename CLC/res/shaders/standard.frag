#version 460 core


out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main()
{           
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * lightColor;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0);
}

/*


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
in vec4 FragPosLightSpace;  // Add this line

out vec4 FragColor;

uniform int numLights;
uniform bool useLighting;
uniform Light lights[MAX_LIGHTS];
uniform vec3 objectColor;
uniform sampler2D texture1;
uniform bool useTexture;
uniform vec3 viewPos;
uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;  // Add this line
uniform vec3 lightPos;  

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // Perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    // Check if the fragment is outside the light's frustum
    if(projCoords.z > 1.0 || projCoords.z < 0.0)
        return 0.0;
        
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
    // Get closest depth value from light's perspective
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    
    // Calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    
    // Check whether current frag pos is in shadow
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}
void main() {
    // Base ambient light
    vec3 resultColor = objectColor * 0.1; 

    // Lighting calculations
    if (useLighting) {
        vec3 norm = normalize(Normal);
        float shadow = ShadowCalculation(FragPosLightSpace);
        
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
                // Spot Light Calculation
                vec3 lightDir = normalize(lights[i].position - FragPos);
                float diff = max(dot(norm, lightDir), 0.0);
                float theta = dot(lightDir, normalize(-lights[i].direction));
                float epsilon = 0.1;
                float intensity = clamp((theta - (1.0 - epsilon)) / epsilon, 0.0, 1.0);
                lightContribution = diff * lights[i].color * lights[i].intensity * intensity;
            }
            
            // Apply shadow to lighting
            lightContribution *= (1.0 - shadow);
            resultColor += lightContribution * objectColor;
        }
    }

    // Apply texture if enabled
    vec4 finalColor = vec4(resultColor, 1.0);
    if (useTexture) {
        vec4 texColor = texture(texture1, TexCoords);
        finalColor.rgb *= texColor.rgb;
        finalColor.a *= texColor.a;
    }

    FragColor = finalColor;
}
*/
