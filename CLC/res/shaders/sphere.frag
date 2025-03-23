#version 460 core

in vec3 FragPos;  // Fragment position in world space
in vec3 Normal;   // Transformed normal

out vec4 FragColor;

uniform vec3 lightPos;    // Light position in world space
uniform vec3 lightColor;  // Light color
uniform vec3 objectColor; // Object color

void main() {
    // Ambient lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse lighting
    vec3 norm = normalize(Normal);  // Normalize the normal
    vec3 lightDir = normalize(lightPos - FragPos);  // Light direction
    float diff = max(dot(norm, lightDir), 0.0);  // Diffuse intensity
    vec3 diffuse = diff * lightColor;

    // Combine ambient and diffuse lighting
    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);  // Output final color
}
