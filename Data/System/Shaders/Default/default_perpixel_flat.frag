#version 410 core

layout (location = 0) flat in vec3 normal;
layout (location = 1) in vec3 vertex;
layout (location = 2) in vec4 vertexColor;

layout(location=0) out vec4 oColor;
layout(location=1) out vec4 oSumColor;
layout(location=2) out vec4 oSumWeight;

// Computations are done in the eye space

vec4 calculateLighting(in vec3 N, in vec3 P, in vec4 vertexColor, in vec4 modelPos);


uniform float uDepthScale = 1.0f;

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
    // Normalize the normal. A varying variable CANNOT 
    // be modified by a fragment shader. So a new variable 
    // needs to be created. 
    vec3 n = normalize( normal ); 
    
    // In this case the built in uniform gl_MaxLights is used 
    // to denote the number of lights. A better option may be passing 
    // in the number of lights as a uniform or replacing the current 
    // value with a smaller value. 
	
	oColor = calculateLighting( n, vertex, vertexColor, vec4(0) );
	oColor.a = 1.0;
	writeWeightedBlendedOITAccumulateBuffer(oColor, gl_FragCoord.w, oSumColor, oSumWeight);
} 
