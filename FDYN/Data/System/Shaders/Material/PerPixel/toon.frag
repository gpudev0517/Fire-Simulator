// Computations are done in the eye space (Eye direction is (0,0,-1))

#version 430 core

uniform float kAmbient;
uniform float kDiffuse;
uniform float kSpecular;
uniform vec4 Color;
uniform vec4 specColor;
uniform bool TwoSided;
uniform vec3 camPosition;

// Raymarch Edge Detection by HLorenzi!
// Detects whether a ray that comes too close to a surface goes away.

#define EDGE_WIDTH 0.5
#define RAYMARCH_ITERATIONS 40

// The traditional method to finding raymarching edges
// Of course, mine has a different purpose (to outline in cartoonish style)
//#define TRADITIONAL




// This should prevent lines from getting thicker when close to the camera
// But it didn't work. Ugh.
//#define PERSPECTIVE_FIX





// Distance functions by www.iquilezles.org
float fSubtraction(float a, float b) {return max(-a,b);}
float fIntersection(float d1, float d2) {return max(d1,d2);}
void fUnion(inout float d1, float d2) {d1 = min(d1,d2);}
float pSphere(vec3 p, float s) {return length(p)-s;}
float pRoundBox(vec3 p, vec3 b, float r) {return length(max(abs(p)-b,0.0))-r;}
float pTorus(vec3 p, vec2 t) {vec2 q = vec2(length(p.xz)-t.x,p.y); return length(q)-t.y;}
float pCapsule(vec3 p, vec3 a, vec3 b, float r) {vec3 pa = p - a, ba = b - a;
	float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 ); return length( pa - ba*h ) - r;}

float distf(vec3 p)
{
	float d = 100000.0;
	
	fUnion(d, pRoundBox(vec3(0,0,10) + p, vec3(11,11,1), 1.0));
	fUnion(d, pRoundBox(vec3(0,0,-2) + p, vec3(5,5,12), 1.0));
	fUnion(d, pSphere(vec3(10,10,0) + p, 8.0));
	fUnion(d, pTorus(vec3(-10,-12,0) + p, vec2(9,5)));
	fUnion(d, pCapsule(p, vec3(-15,15,10), vec3(15,-15,-5), 1.5));
	fUnion(d, -pSphere(p, 80.0));
	
	return d;
}


vec4 raymarch(vec3 from, vec3 increment)
{
	const float maxDist = 200.0;
	const float minDist = 0.001;
	const int maxIter = RAYMARCH_ITERATIONS;
	
	float dist = 0.0;
	
	float lastDistEval = 1e10;
#ifdef TRADITIONAL
	float edge = 1.0;
#else
	float edge = 0.0;
#endif
	
	for(int i = 0; i < maxIter; i++) 
	{
		vec3 pos = (from + increment * dist);
		float distEval = distf(pos);
		
#ifdef TRADITIONAL
		if (distEval < minDist) {
			if (i > RAYMARCH_ITERATIONS - 5) edge = 0.0;
			// Probably should put a break here, but it's not working with GL ES...
		}
#else
	#ifdef PERSPECTIVE_FIX
		// Could not figure out the math :P
		if (lastDistEval < (EDGE_WIDTH / dist) * 20.0 && distEval > lastDistEval + 0.001) {
			edge = 1.0;
		}
	#else
		if (lastDistEval < EDGE_WIDTH && distEval > lastDistEval + 0.001) {
			edge = 1.0;
			// Also should put a break here, but it's not working with GL ES...
		}
	#endif
		if (distEval < minDist) {
			break;
		}
#endif
		
		dist += distEval;
		if (distEval < lastDistEval) lastDistEval = distEval;
	}
	
	return vec4(dist, 0.0, edge, 0);
}

vec4 getPixel(vec3 from, vec3 increment)
{
	vec4 c = raymarch(from, increment);
	return mix(vec4(1,1,1,1),vec4(0,0,0,1),c.z);
}


float stepmix(float edge0, float edge1, float E, float x)
{
    float T = clamp(0.5 * (x - edge0 + E) / E, 0.0, 1.0);
    return mix(edge0, edge1, T);
}

vec4 toon(vec3 N, vec3 L, vec4 lightColor, float intensity, vec4 vertexColor)
{
	float Shininess = 1;
    vec3 Eye = vec3(0, 0, 1);
    vec3 H = normalize(L + Eye);
    
    float df = max(0.0, dot(N, L));
    float sf = max(0.0, dot(N, H));
    sf = pow(sf, Shininess);

    const float A = 0.1;
    const float B = 0.3;
    const float C = 0.6;
    const float D = 1.0;
    float E = fwidth(df);

    if      (df > A - E && df < A + E) df = stepmix(A, B, E, df);
    else if (df > B - E && df < B + E) df = stepmix(B, C, E, df);
    else if (df > C - E && df < C + E) df = stepmix(C, D, E, df);
    else if (df < A) df = 0.0;
    else if (df < B) df = B;
    else if (df < C) df = C;
    else df = D;

    E = fwidth(sf);
    if (sf > 0.5 - E && sf < 0.5 + E)
    {
        sf = smoothstep(0.5 - E, 0.5 + E, sf);
    }
    else
    {
        sf = step(0.5, sf);
    }
	
	vec4 finalColor = Color * lightColor * vertexColor * intensity;
	vec4 shadedColor = kDiffuse * df * finalColor * 0.5 + kSpecular * sf * specColor * 0.5;
	return vec4(shadedColor.rgb, finalColor.a);
}

vec4 ShadingModel( vec3 N, vec3 P, vec3 L, vec4 lightColor, float intensity, vec4 vertexColor, vec3 materialValue )
{
	/*vec3 ro = camPosition;
	vec3 rd = P - camPosition;
	//vec4 col = getPixel( ro, rd );
	vec4 col = getPixel( vec3(0), P );
	return col;*/

	//return toon( N, L, lightColor, intensity, vertexColor );
	
   float normDotL = dot(normalize(N),L);
   vec4 finalColor;
   vec3 c;
   
   if(normDotL <= 0.0)
   {
      if(!TwoSided)
		return vec4(0,0,0,1);
	  else
	    normDotL = -normDotL;
	}
     
    if (normDotL > 0.95)
		finalColor = vertexColor*0.95;
	else if (normDotL > 0.5)
	{
		c = (vertexColor.xyz) * 0.6;
		finalColor = vec4(c,1.0);
	}
	else if (normDotL > 0.25)
	{
		c = (vertexColor.xyz) * 0.4;
		finalColor = vec4(c,1.0);
	}
	else
	{
		c = (vertexColor.xyz) * 0.2;
		finalColor = vec4(c,1.0);
	}
		
	return finalColor;
	
}

vec3 ambientModel(vec3 N, vec3 P, vec4 vertexColor, vec3 materialValue, vec3 localPos)
{
	return vec3(0);
}

vec3 PostProcessing(vec3 color)
{
	return color;
}

vec4 PreProcessing(vec3 N, vec3 P, vec4 vertexColor, vec3 materialValue, vec3 localPos)
{
    return vertexColor;
}