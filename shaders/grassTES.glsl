#version 450 core

layout(quads, equal_spacing, cw) in;

in vec4 tcPosition[];
in vec4 tcCenterPosition[];
in vec4 tcTexCoord[];
in vec4 tcRandoms[];
patch in vec3 controlPoints[2];

out vec3 tePosition;
out vec4 teCenterPosition;
out vec4 teTexCoord;
out vec4 teRandoms;
out vec3 teNormal;

uniform mat4 uMVP;

struct Spline
{
	vec3 position;
	vec3 tangent;
	vec3 a;
};

/**
	De Casteljau's algorithm
	pb - bottom vertex
	pt - top vertex
	h  - additional control point
	v  - domain coordinate
*/
Spline calculateSplinePosition(vec3 pb, vec3 h, vec3 pt, float v)
{
	Spline result;
	
	vec3 a = pb + v * (h - pb);
	vec3 b = h + v * (pt - h);

	result.position = a + v * (b - a);
	result.tangent  = (b - a) / length(b - a);
	result.a 	    = a;

	return result;
}

void main()
{
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	/* Set texture coordinates */
	teTexCoord.st = vec2(u, v);

	/* Calculate position on splines */
	Spline leftSpline  = calculateSplinePosition(tcPosition[0].xyz, controlPoints[0], tcPosition[3].xyz, v);
	Spline rightSpline = calculateSplinePosition(tcPosition[1].xyz, controlPoints[1], tcPosition[2].xyz, v);
    
	/* Calculate final position and normal */
	vec3 splinePos = leftSpline.position * (1.0f - u) + rightSpline.position * u;
	vec3 bitangent = (rightSpline.position - leftSpline.position) / length(rightSpline.position - leftSpline.position * leftSpline.a);
	vec3 tangent   = (leftSpline.tangent * (1.0 - u) + rightSpline.tangent * u) / length(leftSpline.tangent * (1.0 - u) + rightSpline.tangent * u);
	vec3 normal    = cross(tangent, bitangent) / length(cross(tangent, bitangent));
    gl_Position = uMVP * vec4(splinePos, 1.0f);

    tePosition 		 = splinePos;
    teCenterPosition = tcCenterPosition[0];
	teNormal 		 = normal;
    teRandoms		 = tcRandoms[0];
}