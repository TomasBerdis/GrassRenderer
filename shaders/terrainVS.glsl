#version 450 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;
uniform mat4 uMVP;
uniform sampler2D uHeightMap;

void main()
{
    vec4 heightSample = texture(uHeightMap, texCoord);
    float newY = mix(0.0, 30.0, 1 - heightSample.b) ;
    gl_Position = uMVP * vec4(position.x, newY, position.y, 1.0f);
}