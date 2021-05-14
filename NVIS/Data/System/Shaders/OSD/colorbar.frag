#version 120

vec3 getInterpolatedColor(float s);

void main()
{
    float s = gl_TexCoord[0].t;
	gl_FragColor = vec4(getInterpolatedColor(s), 1.0f);
}