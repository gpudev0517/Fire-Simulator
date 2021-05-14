#version 150
#extension GL_EXT_geometry_shader4 : enable

layout(points) in;
layout(triangle_strip, max_vertices=4) out;

out vec4 fs_Passdown1;
out vec4 fs_Passdown2;
out vec4 fs_Passdown3;
out vec4 fs_Passdown4;
out vec4 fs_Color;
out vec4  oModelPos;

void main()
{
	vec4 bounds = gl_TexCoordIn[0][0];
	vec4 ndcPos = gl_TexCoordIn[0][5];

	// frustrum culling
	const float ndcBound = 1.0;
	if (ndcPos.x < -ndcBound) return;
	if (ndcPos.x > ndcBound) return;
	if (ndcPos.y < -ndcBound) return;
	if (ndcPos.y > ndcBound) return;

	float xmin = bounds.x;
	float xmax = bounds.y;
	float ymin = bounds.z;
	float ymax = bounds.w;

	// inv quadric transform
	fs_Passdown1 = gl_TexCoordIn[0][1];
	fs_Passdown2 = gl_TexCoordIn[0][2];
	fs_Passdown3 = gl_TexCoordIn[0][3];
	fs_Passdown4 = gl_TexCoordIn[0][4];
	fs_Color = gl_TexCoordIn[0][6];

	oModelPos = gl_TexCoordIn[0][7];
	
	gl_Position = vec4(xmin, ymax, 0.0, 1.0);
	EmitVertex();

	gl_Position = vec4(xmin, ymin, 0.0, 1.0);
	EmitVertex();

	gl_Position = vec4(xmax, ymax, 0.0, 1.0);
	EmitVertex();

	gl_Position = vec4(xmax, ymin, 0.0, 1.0);
	EmitVertex();
	
}