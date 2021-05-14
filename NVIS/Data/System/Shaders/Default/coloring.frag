#version 130

#define RAMP_MAXSIZE 32

uniform bool interpolationMode; // 0: Linear, 1:Hue
uniform int rampCPCount;
uniform float rampPos[RAMP_MAXSIZE];
uniform vec3 rampColor[RAMP_MAXSIZE];

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float getInterpolation(float start, float end, float s)
{
	return start * s + end * (1-s);
}

vec3 getInterpolation(vec3 startCol, vec3 endCol, float start, float end, float s)
{
	float pos = (end - s) / (end - start);
	
	if (interpolationMode) // Hue
	{
		vec3 minHsv = rgb2hsv(startCol);
		vec3 maxHsv = rgb2hsv(endCol);
		float tmpH = getInterpolation(minHsv.x, maxHsv.x, pos);
		float tmpS = (minHsv.y + maxHsv.y) * 0.5;
		float tmpV = (minHsv.z + maxHsv.z) * 0.5;
		return hsv2rgb(vec3(tmpH, tmpS, tmpV));
	}
	else // Linear
	{
		return startCol * pos + endCol * (1 - pos);
	}
}

vec3 getInterpolatedColor(float s)
{
	if (s >= rampPos[rampCPCount-1])
	{
		return rampColor[rampCPCount-1];
	}
	else if (s < rampPos[0])
	{
		return rampColor[0];
	}
	else
	{
		for(int i = 0; i < rampCPCount-1; i++)
		{
			if (s == rampPos[i])
			{
				return rampColor[i];
			}
			else if (s > rampPos[i] && s < rampPos[i+1])
			{
				return getInterpolation(rampColor[i], rampColor[i+1], rampPos[i], rampPos[i+1], s);
			}
		}
	}
}