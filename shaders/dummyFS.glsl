#version 450 core

in vec2 vTexCoord;
uniform sampler2D debugTexture;
out vec4 color;

void main()
{
    color = texture(debugTexture, vTexCoord);
}