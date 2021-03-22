#version 450 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
uniform mat4 uMVP;
out vec2 vTexCoord;

void main()
{
    gl_Position = uMVP * position;
    vTexCoord = texCoord;
}