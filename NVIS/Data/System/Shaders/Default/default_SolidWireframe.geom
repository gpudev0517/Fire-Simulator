#version 330 core

uniform float threshold;

in VS_GS_INTERFACE
{
	vec3 vertex;
	vec3 clipVert;
	vec4 modelPos;
} gs_in[];

out vec3 gHeights;
out vec3 normal;
out vec4 oModelPos;

layout(triangles_adjacency) in;
layout(triangle_strip, max_vertices = 3) out;

// Compute the triangle face normal from 3 points
vec3 faceNormal(in vec3 posA, in vec3 posB, in vec3 posC)
{
	return normalize( cross(posB - posA, posC - posA) );
}

void main()
{
	float contour_thresh = cos(threshold);
	float Q1, Q2, Q3;
	Q1 = Q2 = Q3 = 0;

	vec3 fn0 = faceNormal(gs_in[0].vertex, gs_in[2].vertex, gs_in[4].vertex);
	vec3 fn1 = faceNormal(gs_in[0].vertex, gs_in[1].vertex, gs_in[2].vertex);
	vec3 fn2 = faceNormal(gs_in[2].vertex, gs_in[3].vertex, gs_in[4].vertex);
	vec3 fn3 = faceNormal(gs_in[4].vertex, gs_in[5].vertex, gs_in[0].vertex);

	normal = vec3(dot(fn0, fn1), dot(fn0, fn2), dot(fn0, fn3));
	if(gs_in[1].vertex != gs_in[2].vertex)
	{
		if(normal.x > contour_thresh){
			Q3 = 1;
		}
	}

	if(gs_in[3].vertex != gs_in[4].vertex)
	{
		if(normal.y > contour_thresh){
			Q1 = 1;
		}
	}

	if(gs_in[0].vertex != gs_in[5].vertex)
	{
		if(normal.z > contour_thresh){
			Q2 = 1;
		}
	}

	
	gl_Position = gl_in[0].gl_Position; gHeights = vec3(1, Q2, Q3); oModelPos = gs_in[0].modelPos; EmitVertex();
	gl_Position = gl_in[2].gl_Position; gHeights = vec3(Q1, 1, Q3); oModelPos = gs_in[2].modelPos; EmitVertex();
	gl_Position = gl_in[4].gl_Position; gHeights = vec3(Q1, Q2, 1); oModelPos = gs_in[4].modelPos; EmitVertex();
	EndPrimitive();
}