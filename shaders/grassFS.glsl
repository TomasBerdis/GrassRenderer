#version 450 core

uniform sampler2D alphaTexture;

in vec4 teTexCoord;
in vec4 teRandoms;
out vec4 color;

void main()
{
    vec4 texColor = texture(alphaTexture, teTexCoord.xy);
    if(texColor.a < 0.1)
        discard;
    else
    {
        vec2 position = (gl_FragCoord.xy / vec2(1920, 1080));

        vec4 top    = vec4(0.086, 0.837, 0.388, 1.0);
        vec4 bottom = vec4(0.086, 0.388, 0.313, 1.0);

        color = vec4(mix(top, bottom, teTexCoord.y));
        color = vec4(color.r + teRandoms.y, color.g + teRandoms.z, color.b + teRandoms.w, color.a);
        // color = vec4(mix(top, bottom, position.y)) * teTexCoord.y;
    }
}