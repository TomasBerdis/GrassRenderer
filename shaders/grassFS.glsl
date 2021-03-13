#version 450 core

// uniform int uWindowWidth;
// uniform int uWindowHeight;

void main()
{
    // int width = uWindowWidth;
    // int height = uWindowHeight;
    vec2 position = (gl_FragCoord.xy / vec2(640, 480));

    vec4 top = vec4(0.0, 1.0, 0.0, 1.0);
    vec4 bottom = vec4(0.0, 0.0, 0.0, 1.0);

    gl_FragColor = vec4(mix(bottom, top, position.y));
}