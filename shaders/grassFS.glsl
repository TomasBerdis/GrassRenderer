#version 450 core

out vec4 color;

void main()
{
    vec2 position = (gl_FragCoord.xy / vec2(1280, 720));

    vec4 top = vec4(0.0, 1.0, 0.0, 1.0);
    vec4 bottom = vec4(0.0, 0.0, 0.0, 1.0);

    color = vec4(mix(bottom, top, position.y));
}