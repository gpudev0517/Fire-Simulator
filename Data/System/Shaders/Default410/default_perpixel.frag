#version 410 core

layout(location = 0) in vec3 normal;
layout(location = 1) in vec3 vertex;
layout(location = 2) in vec4 vertexColor;
layout(location = 3) in vec2 tc0;
layout(location = 4) in vec4 worldPos;
layout(location = 5) in vec3 localPos;

layout(location=0) out vec4 oColor;
layout(location=1) out vec4 oSumColor;
layout(location=2) out vec4 oSumWeight;

// Computations are done in the eye space

vec4 calculateLighting(in vec3 N, in vec3 P, in vec4 vertexColor, in vec4 worldPos, in vec3 materialValue, in vec3 localPos);
bool isClip(vec4 worldPos);
vec4 rigidParticleColor(vec3 fragWorldPos);

uniform vec2 gScreenSize;
uniform bool flipU;
uniform bool flipV;
uniform bool swapUV;
uniform int textured;
uniform bool normalTextured;
uniform bool specTextured;
uniform int repeatU = 1;
uniform int repeatV = 1;

uniform samplerCube cubeMap;
uniform sampler2D colorMap;
uniform sampler2D normalMap;
uniform sampler2D specMap;

uniform int specularWorkflow;

uniform float metallic;
uniform float roughness;
uniform float ao = 1.0f;

uniform float uDepthScale = 1.0f;

uniform mat4 modelview;

uniform bool particleTextured = false;

uniform int kindOIT; // 0 : None, 1 : WeightedBlended, 2 : DepthPeeling Init, 3 : DepthPeeling Peel, 4 : 
uniform sampler2DRect depthPeelingDepthTex;



vec2 CalcScreenTexCoord()
{
    return gl_FragCoord.xy / gScreenSize;
}

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
	if(isClip(worldPos))
		discard;

	vec2 tc = tc0;
	if( swapUV )
	{
		float nu = tc0.x;
		tc.x = tc0.y;
		tc.y = nu;
	}
	if( flipU ) 
	{
		tc.x = 1 - tc.x;
	}
	if( flipV )
	{
		tc.y = 1 - tc.y;
	}
	tc = tc * vec2(repeatU, repeatV);

	// Normalize the normal. A varying variable CANNOT
	// be modified by a fragment shader. So a new variable
	// needs to be created.
	vec3 n = normalize(normal);
	if (!gl_FrontFacing) n = -n;

	if (normalTextured)
	{
		mat3 normalMatrix = mat3(modelview);

		// ----------------------------------------------------------------------------
		// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
		// Don't worry if you don't get what's going on; you generally want to do normal 
		// mapping the usual way for performance anways; I do plan make a note of this 
		// technique somewhere later in the normal mapping tutorial.
		vec3 tangentNormal = texture(normalMap, tc).rgb;

		// transform normal vector to range [-1,1]
    	tangentNormal = normalize(tangentNormal * 2.0 - 1.0);   

		vec3 Q1  = dFdx(worldPos.xyz);
		vec3 Q2  = dFdy(worldPos.xyz);
		vec2 st1 = dFdx(tc);
		vec2 st2 = dFdy(tc);

		vec3 N   = normalize(n);
		vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
		vec3 B  = -normalize(cross(N, T));
		mat3 TBN = mat3(T, B, N);

		n = normalize(TBN * tangentNormal);
	}
	
	vec4 vColor;
	if(textured == 0) 
	{
		vColor = vertexColor;
	}
	else if( textured == 1)
	{
		vec4 tvColor = texture( colorMap, tc );
		vColor = tvColor;
	}
	else if( textured == 2 || textured == 3)
	{
		vec4 cvColor = texture( cubeMap, localPos );
		vColor = cvColor;
	}


	if( particleTextured )
	{
		vColor = rigidParticleColor(vertex);
	}

	vec3 materialValue = vec3(roughness, metallic, ao); // Roughness/Metalness/AO
	if (specTextured)
	{
		materialValue = texture (specMap, tc).rgb;
		materialValue.b = ao;

		if (specularWorkflow % 2 == 1)
		{
			materialValue.x = 1.0 - materialValue.x;
		}
		if (specularWorkflow / 2 == 1)
		{
			materialValue.y = 1.0 - materialValue.y;
		}
	}

	// In this case the built in uniform gl_MaxLights is used
	// to denote the number of lights. A better option may be passing
	// in the number of lights as a uniform or replacing the current
	// value with a smaller value.
	oColor = calculateLighting( n, vertex, vColor, worldPos, materialValue, localPos );
	oColor.a = vColor.a;
	if(kindOIT == 1)
	{
		writeWeightedBlendedOITAccumulateBuffer(oColor, gl_FragCoord.w, oSumColor, oSumWeight);
	}
	else if(kindOIT == 2){
		oColor = vec4(oColor.rgb * oColor.a, 1.0 - oColor.a);
	}
	else if(kindOIT == 3){
		float frontDepth = texture(depthPeelingDepthTex, gl_FragCoord.xy).r;
		if(gl_FragCoord.z <= frontDepth)
			discard;
		oColor = vec4(oColor.rgb * oColor.a, oColor.a);
	}
	
}
