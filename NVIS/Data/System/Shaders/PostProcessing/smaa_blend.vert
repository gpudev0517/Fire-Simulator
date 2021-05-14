#version 410 core

/**
 * Blend Weight Calculation Vertex Shader
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
uniform int SMAA_MAX_SEARCH_STEPS = 32;

void SMAABlendingWeightCalculationVS(float2 texcoord,
                                     out float2 pixcoord,
                                     out float4 offset[3]) {
    pixcoord = texcoord * SMAA_RT_METRICS.zw;

    // We will use these offsets for the searches later on (see @PSEUDO_GATHER4):
    offset[0] = mad(SMAA_RT_METRICS.xyxy, float4(-0.25, -0.125,  1.25, -0.125), texcoord.xyxy);
    offset[1] = mad(SMAA_RT_METRICS.xyxy, float4(-0.125, -0.25, -0.125,  1.25), texcoord.xyxy);

    // And these for the searches, they indicate the ends of the loops:
    offset[2] = mad(SMAA_RT_METRICS.xxyy,
                    float4(-2.0, 2.0, -2.0, 2.0) * float(SMAA_MAX_SEARCH_STEPS),
                    float4(offset[0].xz, offset[1].yw));
}

layout(location=0) out vec2 texcoord;
out vec4 offset[3];
out vec2 pixcoord;

void main()
{
	// uint idx = gl_VertexID % 4; // allows rendering multiple fullscreen rectangles
	// vec4 pos =  vec4(
		// (float( idx     &1U)) * 2.0 - 1.0,
		// (float((idx>>1U)&1U)) * 2.0 - 1.0,
		// 0, 1.0);

	// gl_Position = pos + vec4(SMAA_RT_METRICS.xy, 0, 0);
	// texcoord = pos.xy * 0.5 + 0.5;
    // SMAABlendingWeightCalculationVS(texcoord, pixcoord, offset);
	
	const vec4 vertex[] = vec4[] ( vec4(-1.0, -1.0, 0, 1.0),
                                   vec4( 1.0, -1.0, 0, 1.0),
                                   vec4(-1.0,  1.0, 0, 1.0),
                                   vec4( 1.0,  1.0, 0, 1.0) );
	vec4 pos = vertex[gl_VertexID];
	
	gl_Position = pos + vec4(SMAA_RT_METRICS.xy, 0, 0);
	texcoord = pos.xy * 0.5 + 0.5;
    SMAABlendingWeightCalculationVS(texcoord, pixcoord, offset);
}
