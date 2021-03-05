#version 120

/*
 * Developer : Alexander Dippel (adippel@informatik.uni-freiburg.e)
 * Date : 04/10/2012
 * 
 * This shader applies Screen Space Ambient Occlusion to foam. 
 * Further, the foams intensities are modified using the foams density values.
 * It only runs when the data is correctly assigned via textures. In this textures,
 * the foam's eye space depth values and it's accumulated intensities have to be provided.
 * 
 * The SSAO principle is to create samples in a sphere in eye space uniformly. For each sample
 * where the depth values of the foam, the mesh or a rigid on the sample's  screen space
 * position is lower than the sample's depth, an occlusion is detected so the sample 
 * contributes to the fragment's shadow.
 * 
 */
 
void main()
{ 
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz, 1.0);
    gl_FrontColor = gl_Color;
}