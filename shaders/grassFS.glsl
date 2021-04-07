#version 450 core

uniform sampler2D uAlphaTexture;
uniform vec3 uLightPos;
uniform int uLightingEnabled;

in vec3 tePosition;
in vec4 teTexCoord;
in vec4 teRandoms;
in vec3 teNormal;
out vec4 color;

void main()
{
    vec4 texColor = texture(uAlphaTexture, teTexCoord.st);
    if(texColor.a < 0.1)
        discard;
    else
    {
        vec2 position = (gl_FragCoord.xy / vec2(1920, 1080));

        vec4 top    = vec4(0.086, 0.837, 0.388, 1.0);
        vec4 bottom = vec4(0.086, 0.288, 0.213, 1.0);

        color = vec4(mix(top, bottom, teTexCoord.y));
        color = vec4(color.r + teRandoms.y, color.g + teRandoms.z, color.b + teRandoms.w, color.a);

        /* Lighting */
        if (uLightingEnabled == 1)
        {
            vec3 norm = normalize(teNormal);
            vec3 lightDir = normalize(uLightPos - tePosition);

            float ambientStrength = 0.8;
            vec3 ambient = ambientStrength * vec3(1.0, 1.0, 0.0);

            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * vec3(0.2, 0.2, 0.0);

            vec3 result = (ambient + diffuse) * color.rgb;
            color = vec4(result, 1.0);
        }
    }
}