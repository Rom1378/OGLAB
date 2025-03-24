#version 460 core

in vec3 FragPos;  // Fragment position in world space
in vec3 Normal;   // Transformed normal
in vec2 TexCoords;  // Texture coordinates

out vec4 FragColor;

uniform vec3 lightPos;    // Light position in world space
uniform vec3 lightColor;  // Light color
uniform vec4 objectColor; // Object color (RGBA)

uniform sampler2D texture1;
uniform bool useTexture; // New uniform

void main() {
    // Lighting calculations
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Base color including alpha
    vec4 baseColor = objectColor;
    vec3 result = (ambient + diffuse) * baseColor.rgb;

    // Apply texture if enabled
    if (useTexture) {
        vec4 texColor = texture(texture1, TexCoords);
        result *= texColor.rgb;
        baseColor.a *= texColor.a; // Maintain alpha blending
    }

    FragColor = vec4(result, baseColor.a);
}
