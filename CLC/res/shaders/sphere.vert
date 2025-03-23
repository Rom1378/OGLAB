#version 460 core

layout (location = 0) in vec3 aPos;  // Vertex position
layout (location = 1) in vec3 aNormal;  // Vertex normal

out vec3 FragPos;  // Fragment position in world space
out vec3 Normal;   // Transformed normal

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    // Transform the vertex position
    FragPos = vec3(model * vec4(aPos, 1.0));  // World-space position
    Normal = mat3(model) * aNormal;  // Correct normal transformation

    // Transform the vertex position for rendering
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
