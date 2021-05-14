#version 330 core

// colors fragment according to normal


layout(location = 0) in vec3 N;

layout(location = 0) out vec4 FragColor;

void main (void)
{
	vec3 n = normalize(N);
	if (!gl_FrontFacing) n = -n;
	FragColor = vec4(abs(n.x), abs(n.y), abs(n.z), 1.0);
}