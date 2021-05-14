#version 130

in vec2 texc;

out vec4 color;

uniform sampler2D skySphere;

void main()
{
	color = texture2D(skySphere, texc);
}