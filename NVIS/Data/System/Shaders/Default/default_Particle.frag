#version 430 core

#define MAXCLIP 32

in vec3 posEye;        // position of center in eye space
in float pointSize;
in vec4 vertexColor;
in vec4 worldPos;
flat in int   vertexid;

layout(location=0) out vec4 fragColor;
layout(location=1) out vec4 outFlatWorldPos;

uniform float pointRadius;  // point size in world space
uniform bool lighting;
uniform bool flOpacity;
uniform float intensity;
uniform bool picked;
uniform bool isPCMap;

uniform int nClipPlanes;
uniform vec4 ClipPlanes[MAXCLIP];
uniform bool isProcess[MAXCLIP];
uniform int kindOIT;

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
uniform int MaxNodes;

bool  isClip(vec4 worldPos)
{
	for(int i = 0; i < nClipPlanes; i++)
	{
		if(ClipPlanes[i].x*worldPos.x + ClipPlanes[i].y*worldPos.y + ClipPlanes[i].z*worldPos.z + ClipPlanes[i].w*worldPos.w < 0 && isProcess[i])
			return true;	
	}
	return false;
}

void main()
{
	if(isClip(worldPos))
		discard;

	
	
	if(isPCMap == true)
	{
		
		// // calculate normal from texture coordinates
		// vec3 n;
		// n.xy = gl_PointCoord.xy*vec2(2.0, -2.0) + vec2(-1.0, 1.0);
		// float mag = dot(n.xy, n.xy);
		// if (mag > 1.0) discard;   // kill pixels outside circle
		
		fragColor = vertexColor;
		
		outFlatWorldPos.r = float(vertexid) + 2.0;
		outFlatWorldPos.g = posEye.x;
		outFlatWorldPos.b = posEye.y;
		outFlatWorldPos.a = posEye.z;
	
		return;
	}
	
	float finalIntensity = intensity;

	if(pointSize < 2.0)
	{	
		if(!lighting)
		{
			fragColor = vertexColor;
		}
		else if(picked)
		{
			fragColor = (vertexColor + vec4(0.0, 1.0, 0.0, 1.0))*0.5;
		}
		else
		{
			fragColor = vertexColor * vec4(0.8, 0.8, 0.8, 1.0);		
		}
	}
	else
	{
		//  const vec3 lightDir = vec3(0.577, 0.577, 0.577);
		const vec3 lightDir = vec3(0.301, 0.301,  0.904);
		const float shininess = 40.0;

		// calculate normal from texture coordinates
		vec3 n;
		n.xy = gl_PointCoord.xy*vec2(2.0, -2.0) + vec2(-1.0, 1.0);
		float mag = dot(n.xy, n.xy);
		if (mag > 1.0) discard;   // kill pixels outside circle

		if(picked && mag > 0.5)
		{
			fragColor = vec4(0,1,0,1);
			finalIntensity = 1.0;
		}
		else if(!lighting)
		{
			fragColor = vertexColor;
		}
		else
		{
			n.z = sqrt(1.0-mag);

			// point on surface of sphere in eye space
			vec3 spherePosEye = posEye + n*pointRadius;

			// calculate lighting
			float diffuse = max(0.0, dot(lightDir, n));
			vec3 v = normalize(-spherePosEye);
			vec3 h = normalize(lightDir + v);
			//float specular = pow(max(0.0, dot(n, h)), shininess);
			// vec4 ambient = vec4(0.2, 0.2, 0.2, 1.0);

			//gl_FragColor = vertexColor * diffuse + specular;
			fragColor = vertexColor * diffuse;
		}
	}
	
	//fragColor.a *= finalIntensity;
	fragColor.a = vertexColor.a;

	if(kindOIT == 4) //Linked List
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
		    nodes[nodeIdx].color = fragColor;
		    nodes[nodeIdx].depth = gl_FragCoord.z;
		    nodes[nodeIdx].next = prevHead;

		    
		}
	}
}
