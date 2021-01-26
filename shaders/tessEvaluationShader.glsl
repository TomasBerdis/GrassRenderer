#version 410 core

layout(quads, equal_spacing, cw) in;

patch in vec3 controlPoints[2];

in vec3 tcPosition[];

struct SplineData
{
	vec3 pos;
	vec3 tangent;
	vec3 a;
};

// De Casteljau's
SplineData calcSplinePos(vec3 p1, vec3 p2, vec3 p3, float param)
{
	SplineData result;
	
	vec3 a = p1 + param*(p2 - p1);
	vec3 b = p2 + param*(p3 - p2);

	result.pos = a + param*(b - a);
	result.tangent = (b - a) / length(b - a);
	result.a = a;

	return result;
}

void main()
{
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	SplineData leftSpline = calcSplinePos(tcPosition[0], controlPoints[0], tcPosition[1], v);
	SplineData rightSpline = calcSplinePos(tcPosition[3], controlPoints[1], tcPosition[2], v);
    
	vec3 splinePos = leftSpline.pos*(1.0f - u) + rightSpline.pos*u;

    gl_Position = vec4(splinePos, 1.0f);
    // gl_Position =   (gl_TessCoord.x * gl_in[0].gl_Position) +
    //                 (gl_TessCoord.y * gl_in[1].gl_Position);
    // vec3 a = mix(tcPosition[0], tcPosition[1], u);
    // vec3 b = mix(tcPosition[2], tcPosition[3], u);
    // tePosition = mix(a, b, v);
    // gl_Position = vec4(tePosition, 1.0f);
}