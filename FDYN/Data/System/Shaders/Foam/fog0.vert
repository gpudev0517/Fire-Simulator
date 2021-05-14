#version 120

// Main function
void main()
{
	gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;
	
	vec3 posEye = vec3(gl_ClipVertex);
	float invDist = 1.0/abs(posEye.z);
	gl_PointSize = 100.0*invDist;
	
	gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz, 1.0);
	gl_FrontColor = gl_Color;
	return;
}