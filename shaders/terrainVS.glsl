#version 450 core

layout(location = 0) in vec2 position;

uniform mat4 uMVP;
uniform float uFieldSize;
uniform float uMaxTerrainHeight;
uniform sampler2D uHeightMap;

void main()
{
    float x =      (position.x + uFieldSize/2) / uFieldSize;    // normalize x (possitive x is pointing away from us)
    float z = 1 - ((position.y + uFieldSize/2) / uFieldSize);   // normalize z (possitive z is pointing towards us)
    vec2 mapCoords = vec2(x, z);
    vec4 heightSample = texture(uHeightMap, mapCoords);
    float newY  = 0.0f + mix(0.0, uMaxTerrainHeight, 1 - heightSample.b);
    gl_Position = uMVP * vec4(position.x, newY, position.y, 1.0f);
}