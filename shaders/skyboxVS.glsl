#version 450 core

layout(location = 0) in vec3 position;

out vec3 vTexCoord;

uniform mat4 uMVP;

void main()
{
    vTexCoord = position;
    gl_Position = uMVP * vec4(position, 1.0);
} 