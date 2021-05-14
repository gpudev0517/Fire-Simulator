#version 130

in vec3 worldPos;

out vec4 FragColor;

uniform float upperValue;
uniform float lowerValue;
uniform float layerheight;

vec3 getInterpolatedColor(float s);

void main()
{
	if( worldPos.y < layerheight)
		FragColor = vec4(getInterpolatedColor(lowerValue), 0.5f);
	else
		FragColor = vec4(getInterpolatedColor(upperValue), 0.5f);;
}