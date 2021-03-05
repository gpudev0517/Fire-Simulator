#version 330 core
in vec2 fragVel;
out vec4 color;
const float maxVelocity = 12.0;

void main()
{
	color = vec4((fragVel.x/maxVelocity + 1.0)*0.5, (fragVel.y/maxVelocity + 1.0)*0.5 ,0,1); 
}