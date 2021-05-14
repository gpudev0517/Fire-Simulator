#version 150

#define MAXCLIP 32

uniform vec3 invViewport;
uniform vec3 invProjection;
uniform mat4 projection;

uniform int nClipPlanes;
uniform vec4 ClipPlanes[MAXCLIP];
uniform bool isProcess[MAXCLIP];

in vec4 fs_Passdown1;
in vec4 fs_Passdown2;
in vec4 fs_Passdown3;
in vec4 fs_Passdown4;
in vec4 fs_Color;
in vec4  oModelPos;

out float out_Depth;
out vec4 out_Color;

bool  isClip(vec4 worldPos)
{
    for(int i = 0; i < nClipPlanes; i++)
    {
        if(ClipPlanes[i].x*worldPos.x + ClipPlanes[i].y*worldPos.y + ClipPlanes[i].z*worldPos.z + ClipPlanes[i].w*worldPos.w < 0 && isProcess[i])
            return true;    
    }
    return false;
}

float Sign(float x) { return x < 0.0 ? -1.0: 1.0; }

bool solveQuadratic(float a, float b, float c, out float minT, out float maxT)
{
	if (a == 0.0 && b == 0.0)
	{
		minT = maxT = 0.0;
		return true;
	}

	float discriminant = b*b - 4.0*a*c;

	if (discriminant < 0.0)
	{
		return false;
	}

	float t = -0.5*(b + Sign(b)*sqrt(discriminant));
	minT = t / a;
	maxT = c / t;

	if (minT > maxT)
	{
		float tmp = minT;
		minT = maxT;
		maxT = tmp;
	}

	return true;
}

float sqr(float x) { return x*x; }

void main()
{
    if(isClip(oModelPos))
        discard;
	// transform from view space to parameter space
	mat4 invQuadric;
	invQuadric[0] = fs_Passdown1;
	invQuadric[1] = fs_Passdown2;
	invQuadric[2] = fs_Passdown3;
	invQuadric[3] = fs_Passdown4;

	vec4 ndcPos = vec4(gl_FragCoord.xy*invViewport.xy*vec2(2.0, 2.0) - vec2(1.0, 1.0), -1.0, 1.0);
	mat4 invproj = inverse(projection); 
	
	vec4 viewDir = invproj*ndcPos; 

	// ray to parameter space
	vec4 dir = invQuadric*vec4(viewDir.xyz, 0.0);
	vec4 origin = invQuadric[3];

	// set up quadratric equation
	float a = sqr(dir.x) + sqr(dir.y) + sqr(dir.z);// - sqr(dir.w);
	float b = dir.x*origin.x + dir.y*origin.y + dir.z*origin.z - dir.w*origin.w;
	float c = sqr(origin.x) + sqr(origin.y) + sqr(origin.z) - sqr(origin.w);

	float minT;
	float maxT;

	if (solveQuadratic(a, 2.0*b, c, minT, maxT))
	{
		vec3 eyePos = viewDir.xyz*minT;
		vec4 ndcPos = projection * vec4(eyePos, 1.0);
		ndcPos.z /= ndcPos.w;

		out_Depth = eyePos.z;
		out_Color = fs_Color;
		gl_FragDepth = ndcPos.z*0.5 + 0.5;

		return;
	}
	else
		discard;

	out_Depth = 0.5;
	out_Color = vec4(0,0,0,0);
}