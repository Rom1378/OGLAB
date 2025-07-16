#version 460 core

layout(location = 0) in vec3 startPos;
layout(location = 1) in vec3 endPos;
layout(location = 2) in vec4 color;
layout(location = 2) in float thickness;

out vec3 Color;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0);
    Color = aColor;
    Thickness = thickness;
}