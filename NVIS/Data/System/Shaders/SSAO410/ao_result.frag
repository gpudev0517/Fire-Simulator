#version 410 core

in vec2 texCoord;

layout(location=0) out vec4 oColor;
layout(location=1) out vec4 oSumColor;
layout(location=2) out vec4 oSumWeight;

uniform sampler2D aoMap;

uniform bool isMultiSample;

uniform sampler2DMS colorMapMS;
uniform sampler2DMS depthMapMS;

uniform sampler2D colorMap;
uniform sampler2D depthMap;

#define BIAS 0.0001

uniform float uDepthScale = 1.0f;
uniform int kindOIT; // 0 : None, 1 : WeightedBlended, 2 : DepthPeeling Init, 3 : DepthPeeling Peel, 4 : 

void writeWeightedBlendedOITAccumulateBuffer(vec4 color, float fragCoordW, out vec4 sumColor, out vec4 sumWeight)
{
	// Assuming that the projection matrix is a perspective projection
    // gl_FragCoord.w returns the inverse of the oPos.w register from the vertex shader
    float viewDepth = abs(1.0 / fragCoordW);

    // Tuned to work well with FP16 accumulation buffers and 0.001 < linearDepth < 2.5
    // See Equation (9) from http://jcgt.org/published/0002/02/09/
    float linearDepth = viewDepth * uDepthScale;
    float weight = clamp(0.03 / (1e-5 + pow(linearDepth, 4.0)), 1e-2, 3e3);

	sumColor = vec4(color.rgb * color.a, color.a) * weight;
	sumWeight = vec4(color.a);
}

void main()
{
	//float fdepth = texture( depthMap, texCoord).x;
	float fdepth;
	if(isMultiSample)
		fdepth = texelFetch(depthMapMS, ivec2(gl_FragCoord.xy), 0).x;
	else
		fdepth = texelFetch(depthMap, ivec2(gl_FragCoord.xy), 0).x;
	
	if(fdepth >= 1.0) 
		discard;
	//vec4 vColor = texture( colorMap, texCoord);
	vec4 vColor = vec4(0,0,0,0);
	if(isMultiSample)
		vColor = texelFetch( colorMapMS, ivec2(gl_FragCoord.xy), 0);
	else
		vColor = texelFetch( colorMap, ivec2(gl_FragCoord.xy), 0);
	
	vec4 vAO = texture( aoMap, texCoord);

	fdepth = fdepth - BIAS;
	if (fdepth < 0)
		fdepth = 0;

	oColor = vColor * vAO;
	oColor.a = 1.0 - vAO.x;

	if(kindOIT == 0)
	{
		// nothing to do 
	}
	else if(kindOIT == 1)
	{
		writeWeightedBlendedOITAccumulateBuffer(oColor, 1/fdepth, oSumColor, oSumWeight);
	}
	else if(kindOIT == 2){
		oColor = vec4(oColor.rgb * oColor.a, 1.0 - oColor.a);
	}
	else if(kindOIT == 3){
		// nothing, because this shader is not running when kindOIT == 3
	}
	
	gl_FragDepth = fdepth;	
}