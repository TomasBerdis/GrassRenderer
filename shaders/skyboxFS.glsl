#version 450 core

in vec3 vTexCoord;

out vec4 color;

uniform samplerCube uSkybox;

void main()
{
    color = texture(uSkybox, vTexCoord);
}