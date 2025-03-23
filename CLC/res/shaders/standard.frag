#version 460 core

in vec3 FragPos;  // Fragment position in world space
in vec3 Normal;   // Transformed normal
in vec2 TexCoords;  // Texture coordinates

out vec4 FragColor;


uniform vec3 lightPos;    // Light position in world space
uniform vec3 lightColor;  // Light color
uniform vec3 objectColor; // Object color

uniform sampler2D texture1;
uniform bool useTexture; // New uniform

void main() {
    // Lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Base color
    vec3 result = (ambient + diffuse) * objectColor;

    // Apply texture if enabled
    if (useTexture) {
        vec4 texColor = texture(texture1, TexCoords);
        result *= texColor.rgb;
    }

    FragColor = vec4(result, 1.0);
}
