#version 120

void main()
{
    // Input vertex position is in viewport coordinate system (0~1)
  gl_Position = vec4(gl_Vertex.xy * 2 - vec2(1), 0, 1);
  float zPos = gl_Vertex.z;
  if (zPos < 1)
    gl_TexCoord[0] = vec4(0,0,0,1);
  else if (zPos < 2)
    gl_TexCoord[0] = vec4(1,0,0,1);
  else if (zPos < 3)
    gl_TexCoord[0] = vec4(1,1,0,1);
  else if (zPos < 4)
    gl_TexCoord[0] = vec4(0,1,0,1);
}