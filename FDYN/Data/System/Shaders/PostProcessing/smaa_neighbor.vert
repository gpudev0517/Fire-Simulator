#version 410 core
/**
 * Neighborhood Blending Vertex Shader
 */
#define float2 vec2
#define float3 vec3
#define float4 vec4
#define int2 ivec2
#define int3 ivec3
#define int4 ivec4
#define bool2 bvec2
#define bool3 bvec3
#define bool4 bvec4

#if defined(SMAA_GLSL_4)
#define mad(a, b, c) fma(a, b, c)
#else
#define mad(a, b, c) (a * b + c)
#endif

uniform vec4 SMAA_RT_METRICS = float4(1.0 / 1280.0, 1.0 / 720.0, 1280.0, 720.0);

void SMAANeighborhoodBlendingVS(float2 texcoord, out float4 offset) {
    offset = mad(SMAA_RT_METRICS.xyxy, float4( 1.0, 0.0, 0.0,  1.0), texcoord.xyxy);
}

layout(location=0) out vec2 texcoord;
out vec4 offset;
void main()
{
	// uint idx = gl_VertexID % 4; // allows rendering multiple fullscreen rectangles
	// vec4 pos =  vec4(
		// (float( idx     &1U)) * 2.0 - 1.0,
		// (float((idx>>1U)&1U)) * 2.0 - 1.0,
		// 0, 1.0);

	// gl_Position = pos + vec4(SMAA_RT_METRICS.xy, 0, 0);
	// texcoord = pos.xy * 0.5 + 0.5;
    // SMAANeighborhoodBlendingVS(texcoord, offset);
	
	const vec4 vertex[] = vec4[] ( vec4(-1.0, -1.0, 0, 1.0),
                                   vec4( 1.0, -1.0, 0, 1.0),
                                   vec4(-1.0,  1.0, 0, 1.0),
                                   vec4( 1.0,  1.0, 0, 1.0) );
    vec4 pos = vertex[gl_VertexID];
	
	gl_Position = pos + vec4(SMAA_RT_METRICS.xy, 0, 0);
	texcoord = pos.xy * 0.5 + 0.5;
    SMAANeighborhoodBlendingVS(texcoord, offset);
}
