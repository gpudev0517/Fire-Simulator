#version 120

uniform int targetChannel;
varying float viewDepth;

void main()
{
	vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
	color[targetChannel] = viewDepth;
	gl_FragData[0] = color;
}