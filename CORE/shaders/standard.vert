#version 460 core




layout (location = 0) in vec3 aPos;  // Vertex position
layout (location = 1) in vec3 aNormal;  // Vertex normal
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;  // Fragment position in world space
out vec3 Normal;   // Transformed normal
out vec2 TexCoords;
out vec4 FragPosLightSpace;  // Add this output

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;  // Add this uniform

void main() {
    // Transform the vertex position
    FragPos = vec3(model * vec4(aPos, 1.0));  // World-space position
    Normal = mat3(transpose(inverse(model))) * aNormal;  // Correct normal transformation
    TexCoords = aTexCoord;
    
    // Calculate fragment position in light space for shadow mapping
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    
    // Transform the vertex position for rendering
    gl_Position = projection * view * vec4(FragPos, 1.0);
}