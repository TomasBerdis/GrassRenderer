#version 450 core

// output patch with vertex count of 4
layout(vertices = 4) out;

in vec4 vPosition[];
in vec4 vCenterPosition[];
in vec4 vTexCoord[];
in vec4 vRandoms[];

out vec4 tcPosition[];
out vec4 tcCenterPosition[];
out vec4 tcTexCoord[];
out vec4 tcRandoms[];
patch out vec3 controlPoints[2];

uniform int uMaxTessLevel;
uniform float uMaxDistance;
uniform vec3 uCameraPos;
uniform mat4 uMVP;

vec3 calcControlPoint(vec4 lower, vec4 upper)
{
	float r3 = vTexCoord[0].w;   // r3
	float r4 = vRandoms[0].x;    // r4

	float x = lower.x * r3 + upper.x * (1.0 - r3);
	float y = lower.y * r4 + upper.y * (1.0 - r4);
	float z = lower.z * r3 + upper.z * (1.0 - r3);
	return vec3(x, y, z);
}

void main()
{
    /* Calculate distance to camera */
    float cameraDistance = length(vPosition[0].xyz - uCameraPos);

    float tessellationLevel = ceil(uMaxTessLevel * (1 - (cameraDistance / uMaxDistance)));

    /* Randomly discard blades based on distance */
    float r = vCenterPosition[0].w + (cameraDistance / uMaxDistance);
    if (r > 1)
        tessellationLevel = 0;

    if (gl_InvocationID == 0)
    {
        // if any of the outer levels is zero, the patch is culled
        gl_TessLevelOuter[0] = tessellationLevel;   // left edge
        gl_TessLevelOuter[1] = 1.0f;                // bottom edge
        gl_TessLevelOuter[2] = tessellationLevel;   // right edge
        gl_TessLevelOuter[3] = 1.0f;                // top edge
        
        gl_TessLevelInner[0] = tessellationLevel;   // top and bottom
        gl_TessLevelInner[1] = tessellationLevel;   // left and right
        
		controlPoints[0] = calcControlPoint(vPosition[3], vPosition[0]);
		controlPoints[1] = calcControlPoint(vPosition[2], vPosition[1]);
    }

    tcPosition[gl_InvocationID]       = vPosition[gl_InvocationID];
    tcCenterPosition[gl_InvocationID] = vCenterPosition[gl_InvocationID];
    tcTexCoord[gl_InvocationID]       = vTexCoord[gl_InvocationID];
    tcRandoms[gl_InvocationID]        = vRandoms[gl_InvocationID];
}