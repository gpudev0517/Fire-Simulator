#version 120

/*
 * Developer : Alexander Dippel (adippel@informatik.uni-freiburg.e)
 * Date : 04/10/2012
 * 
 * This shader filters shadow values produced by SSAO. 
 * A gauﬂian filter is applied to achieve this.
 * In the end, the shadows and intensities are merged with a pre-rendered
 * backround image.
 * 
 */

void main()
{ 
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz, 1.0);
    gl_FrontColor = gl_Color;
}