#version 430 core

in vec4 color;
in vec4 modelPos;

layout(location=0) out vec4 FragColor;
layout(location=1) out vec4 oSumColor;
layout(location=2) out vec4 oSumWeight;

uniform int kindOIT; // 0 : None, 1 : WeightedBlended, 2 : DepthPeeling Init, 3 : DepthPeeling Peel, 4 : Linked List
uniform int MaxNodes;
uniform sampler2DRect depthPeelingDepthTex;

struct NodeType {
  vec4 color;
  float depth;
  uint next;
};

layout( binding = 0, r32ui) uniform uimage2D headPointers;
layout( binding = 0, offset = 0) uniform atomic_uint nextNodeCounter;
layout( binding = 0, std430 ) buffer BufferObject {
  NodeType nodes[];
};

#define MAXCLIP 32
uniform int nClipPlanes;
uniform vec4 ClipPlanes[MAXCLIP];
uniform bool isProcess[MAXCLIP];

uniform bool isClipMode = false;

bool  isClip(vec4 worldPos)
{
	for(int i = 0; i < nClipPlanes; i++)
	{
		if(ClipPlanes[i].x*worldPos.x + ClipPlanes[i].y*worldPos.y + ClipPlanes[i].z*worldPos.z + ClipPlanes[i].w*worldPos.w < 0 && isProcess[i])
			return true;	
	}
	return false;
}

void writeWeightedBlendedOITAccumulateBuffer(vec4 color, float fragCoordW, out vec4 sumColor, out vec4 sumWeight)
{
	// Assuming that the projection matrix is a perspective projection
    // gl_FragCoord.w returns the inverse of the oPos.w register from the vertex shader
    float viewDepth = abs(1.0 / fragCoordW);

    // Tuned to work well with FP16 accumulation buffers and 0.001 < linearDepth < 2.5
    // See Equation (9) from http://jcgt.org/published/0002/02/09/
    float linearDepth = viewDepth; // * uDepthScale; // default value is 1.0f
    float weight = clamp(0.03 / (1e-5 + pow(linearDepth, 4.0)), 1e-2, 3e3);

	sumColor = vec4(color.rgb * color.a, color.a) * weight;
	sumWeight = vec4(color.a);
}

void main()
{
	if(isClip(modelPos))
		discard;

	FragColor = color;
	if(kindOIT == 1)
	{
		writeWeightedBlendedOITAccumulateBuffer(FragColor, gl_FragCoord.w, oSumColor, oSumWeight);
	}
	else if(kindOIT == 2){
		FragColor = vec4(color.rgb  * color.a, 1.0 - color.a);
	}
	else if(kindOIT == 3){
		float frontDepth = texture(depthPeelingDepthTex, gl_FragCoord.xy).r;
		if(gl_FragCoord.z <= frontDepth)
			discard;
		FragColor = vec4(color.rgb  * color.a, color.a);		
	}
	else if(kindOIT == 4) //Linked List
	{

		// Get the index of the next empty slot in the buffer
		uint nodeIdx = atomicCounterIncrement(nextNodeCounter);

		// Is our buffer full?  If so, we don't add the fragment
		// to the list.
		if( nodeIdx < MaxNodes ) {

		    // Our fragment will be the new head of the linked list, so
		    // replace the value at gl_FragCoord.xy with our new node's
		    // index.  We use imageAtomicExchange to make sure that this
		    // is an atomic operation.  The return value is the old head
		    // of the list (the previous value), which will become the
		    // next element in the list once our node is inserted.
		    uint prevHead = imageAtomicExchange(headPointers, ivec2(gl_FragCoord.xy), nodeIdx);

		    // Here we set the color and depth of this new node to the color
		    // and depth of the fragment.  The next pointer, points to the
		    // previous head of the list.
		    nodes[nodeIdx].color = FragColor;
		    nodes[nodeIdx].depth = gl_FragCoord.z;
		    nodes[nodeIdx].next = prevHead;
		}
	}
	else{
		FragColor = color;
	}
}