#version 130

out vec2 texCoord;
out vec4 worldPos;

uniform mat4 modelviewprojection;
uniform float radial;

void main()
{
  // uint idx = gl_VertexID % 4; // allows rendering multiple fullscreen triangles
  // vec4 pos =  vec4(
	  // (float( idx     &1U)) * 4.0 - 2.0,
	  // -0.001, 
	  // (float((idx>>1U)&1U)) * 4.0 - 2.0,
	  // 1.0);
  
  // texCoord = 0.5 * pos.xz * 0.5 + 0.5;
  
  // pos = 0.5 * radial * pos;
  // pos.w = 1.0;
  // worldPos = pos;
  // gl_Position = modelviewprojection * pos;
  
  const vec4 vertex[] = vec4[] ( vec4(-1.0, 0, -1.0, 1.0),
                                   vec4( 1.0, 0, -1.0, 1.0),
                                   vec4(-1.0, 0, 1.0, 1.0),
                                   vec4( 1.0, 0, 1.0, 1.0) );

  vec4 pos = vertex[gl_VertexID];

  texCoord = pos.xz * 0.5 + 0.5;								   
  pos  = radial * pos;
  pos.w = 1.0;
  worldPos = pos;
  gl_Position = modelviewprojection * pos;
  
}


