#version 460 core
layout(location = 0) in vec3 aPos;

out vec3 localPos;  // Consistent naming

uniform mat4 projection;
uniform mat4 view;

void main() {
    localPos = aPos;
    gl_Position = projection * view * vec4(aPos, 1.0);
}