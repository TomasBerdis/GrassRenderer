#version 450 core

layout(location = 0) in vec2 position;

uniform mat4 uMVP;
uniform float uMaxTerrainHeight;
uniform float uTerrainWidth;
uniform float uTerrainHeight;
uniform sampler2D uHeightMap;

void main()
{
    float x =      (position.x + uTerrainWidth  / 2) / uTerrainWidth;     // normalize x (possitive x is pointing away from us)
    float z = 1 - ((position.y + uTerrainHeight / 2) / uTerrainHeight);   // normalize z (possitive z is pointing towards us)
    x = clamp(x, 0.01, 0.99);
    z = clamp(z, 0.01, 0.99);
    vec2 mapCoords = vec2(x, z);
    vec4 heightSample = texture(uHeightMap, mapCoords);
    float newY  = 0.0f + mix(0.0, uMaxTerrainHeight, 1 - heightSample.b);
    gl_Position = uMVP * vec4(position.x, newY, position.y, 1.0f);
}