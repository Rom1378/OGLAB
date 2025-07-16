#version 460 core



out vec3 Color;


in vec4 Color;
out vec4 FragColor;

void main() {
    FragColor = Color;
}