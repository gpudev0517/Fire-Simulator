#version 120

varying float viewDepth;

void main()
{
	gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;
	viewDepth = -gl_ClipVertex.z;
    gl_Position = ftransform();
}