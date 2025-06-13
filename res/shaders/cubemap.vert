#version 460 core
layout(location = 0) in vec3 aPos;

out vec3 TexCoords;  // Changed from WorldPos to match fragment shader

uniform mat4 projection;
uniform mat4 view;

void main() {
    TexCoords = aPos;
    mat4 rotView = mat4(mat3(view)); // Remove translation
    vec4 clipPos = projection * rotView * vec4(aPos, 1.0);
    gl_Position = clipPos.xyww;
}