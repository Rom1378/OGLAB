#version 460 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 color;




out vec4 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = model * projection * view * vec4(pos, 1.0);
    Color = color;
}