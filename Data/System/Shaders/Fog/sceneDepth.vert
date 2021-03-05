#version 120

varying float viewDepth;

void main()
{
	gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;
	viewDepth = -gl_ClipVertex.z;
	//viewDepth = 0.7;
    gl_Position = ftransform();
}