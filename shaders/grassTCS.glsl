#version 450 core

// output patch with vertex count of 4
layout(vertices = 4) out;

patch out vec3 controlPoints[2];

in vec3 vPosition[];
in vec3 vCenterPosition[];

out vec3 tcPosition[];
out vec3 tcCenterPosition[];

uniform int uTessLevel;
uniform mat4 uMVP;

vec3 calcControlPoint(vec3 lower, vec3 upper)
{
	// scaled between -1/4 & 1/4
	float rand1 = 0.1f;
	// scaled between 3/4 & 5/4
	float rand2 = 0.75f;

	float x = lower.x*rand1 + upper.x*(1.0 - rand1);
	float y = lower.y*rand2 + upper.y*(1.0 - rand2);
	float z = lower.z*rand1 + upper.z*(1.0 - rand1);
	return vec3(x, y, z);
}

void main()
{
    float tessellationLevel = uTessLevel;

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

    tcPosition[gl_InvocationID] = vPosition[gl_InvocationID];
    tcCenterPosition[gl_InvocationID] = vCenterPosition[gl_InvocationID];

}