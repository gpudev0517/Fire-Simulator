#version 130

in vec2 texCoord;
in vec4 worldPos;

out vec4 FragColor;

uniform vec3 realColor;

#define sqr_0_dot_5 0.7

vec4 calculateVisibility(in vec4 vertexPosition);

void main()
{
	vec2 vDir = texCoord - vec2(0.5, 0.5);
	float dist = pow(vDir.x * vDir.x + vDir.y * vDir.y, 0.25);
	//float dist = sqrt(vDir.x * vDir.x + vDir.y * vDir.y);
	
	if(dist >= sqr_0_dot_5)
		dist = sqr_0_dot_5;
	FragColor.rgb = realColor * calculateVisibility(worldPos).rgb;
	FragColor.a = 1 - dist / sqr_0_dot_5 ;
	//FragColor.a = 1;
}