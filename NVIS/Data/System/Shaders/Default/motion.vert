#version 330 core
in vec3 position;
in vec3 vel;
out vec2 fragVel;
uniform mat4 viewMatrix;

uniform mat4 modelview;
uniform mat4 projection;

void main()
{
	// Transform the vertex position to view space
	vec4 clipVertex = modelview * vec4(position, 1.0);

	// Make the model-view-projection transform
	gl_Position = projection * clipVertex;

	vec4 velView = viewMatrix * vec4(vel, 0.0);
	vec3 vel3 = vec3(velView.xy, clipVertex.z);
	vec4 velT = projection * vec4(vel3, 1.0);
	fragVel = velT.xy/velT.w;
}