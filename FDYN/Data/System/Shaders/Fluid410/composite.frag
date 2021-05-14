#version 410 core

uniform vec2 invTexScale;
uniform vec3 lightDir;

uniform vec2 clipPosToEye;
uniform mat4 projection;

uniform sampler2D tex;
uniform sampler2D colorTex;

uniform bool debug;

in vec2 texCoord;


uniform float opacityAlpha;
uniform int kindOIT;


vec4 calculateLighting(in vec3 N, in vec3 P, in vec4 vertexColor, in vec4 worldPos, in vec3 materialValue, in vec3 localPos);

vec3 viewportToEyeSpace(vec2 coord, float eyeZ)
{
	// find position at z=1 plane
	vec2 uv = (coord*2.0 - vec2(1.0))*clipPosToEye;

	return vec3(-uv*eyeZ, eyeZ);
}

vec3 getInvertColor(vec3 color)
{
	float y = dot(vec3(0.299, 0.587, 0.114), color);
	if (y >= 0.5) return vec3(0);
	return vec3(1);
}

void main()
{
	float eyeZ = texture2D(tex, texCoord).x;

	if (eyeZ == 0.0)
		discard;

	// reconstruct eye space pos from depth
	vec3 eyePos = viewportToEyeSpace(texCoord, eyeZ);

	// finite difference approx for normals, can't take dFdx because
	// the one-sided difference is incorrect at shape boundaries
	vec3 zl = eyePos - viewportToEyeSpace(texCoord - vec2(invTexScale.x, 0.0), texture2D(tex, texCoord - vec2(invTexScale.x, 0.0)).x);
	vec3 zr = viewportToEyeSpace(texCoord + vec2(invTexScale.x, 0.0), texture2D(tex, texCoord + vec2(invTexScale.x, 0.0)).x) - eyePos;
	vec3 zt = viewportToEyeSpace(texCoord + vec2(0.0, invTexScale.y), texture2D(tex, texCoord + vec2(0.0, invTexScale.y)).x) - eyePos;
	vec3 zb = eyePos - viewportToEyeSpace(texCoord - vec2(0.0, invTexScale.y), texture2D(tex, texCoord - vec2(0.0, invTexScale.y)).x);
	
	vec3 dx = zl;
	vec3 dy = zt;

	if (abs(zr.z) < abs(zl.z))
		dx = zr;

	if (abs(zb.z) < abs(zt.z))
		dy = zb;
	
	vec4 color = texture2D(colorTex, texCoord);
	
	vec3 n = normalize(cross(dx, dy));

	gl_FragColor = calculateLighting(n, eyePos, color, vec4(0), vec3(1), vec3(0));

	if (debug)
		gl_FragColor = vec4(n*0.5 + vec3(0.5), 1.0);

	// write valid z
	vec4 clipPos = projection*vec4(0.0, 0.0, eyeZ, 1.0);
	clipPos /= clipPos.w;

	gl_FragDepth = clipPos.z*0.5 + 0.5;

	vec4 fragColor = gl_FragColor;
    fragColor.a = opacityAlpha;

}