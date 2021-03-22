#version 450 core

layout(quads, equal_spacing, cw) in;

patch in vec3 controlPoints[2];

in vec4 tcPosition[];
in vec4 tcCenterPosition[];
patch in vec4 tcTexCoord;
in vec4 tcRandoms[];

out vec4 tePosition;
out vec4 teCenterPosition;
out vec4 teTexCoord;
out vec4 teRandoms;

uniform mat4 uMVP;

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
	
	vec3 a = p1 + param * (p2 - p1);
	vec3 b = p2 + param * (p3 - p2);

	result.pos 	   = a + param * (b - a);
	result.tangent = (b - a) / length(b - a);
	result.a 	   = a;

	return result;
}

void main()
{
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	// vec2 a = mix(tcTexCoord[0].xy, tcTexCoord[3].xy, u);
	// vec2 b = mix(tcTexCoord[1].xy, tcTexCoord[2].xy, u);
	// teTexCoord.xy = mix(a, b, v);

	SplineData leftSpline  = calcSplinePos(tcPosition[0].xyz, controlPoints[0], tcPosition[3].xyz, v);
	SplineData rightSpline = calcSplinePos(tcPosition[1].xyz, controlPoints[1], tcPosition[2].xyz, v);
    
	vec3 splinePos = leftSpline.pos * (1.0f - u) + rightSpline.pos * u;

    gl_Position = uMVP * vec4(splinePos, 1.0f);

    tePosition 		 = gl_Position;
    teCenterPosition = tcCenterPosition[0];
    teTexCoord		 = tcTexCoord;
    teRandoms		 = tcRandoms[0];
}