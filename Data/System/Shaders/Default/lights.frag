uniform bool isReceiveShadow = false;
/*
	The lights for the shaders are filled and maintained by FLRenderer
*/

struct SpotLight
{
	bool isactive;
	vec3 pos;
	vec3 dir;
	vec4 color;
	float intensity;
	float cosCutoff;
	float exponent;
	float attLin;
	float attQuad;
	sampler2D shadowMap;
    mat4 DepthBiasMVP;
    int castingShadow;
    vec4 shadowColor;
    mat4 lightView;
    vec2 lightRadiusUV;
    float lightZNear;
    float lightZFar;
};

struct PointLight
{
	bool isactive;
	vec3 pos;
	vec4 color;
	float intensity;
	float attLin;
	float attQuad;
};

struct DirLight
{
	bool isactive;
	vec3 dir;
	vec4 color;
	float intensity;
	sampler2D shadowMap;
	mat4 DepthBiasMVP;
	int castingShadow;
	vec4 shadowColor;
	mat4 lightView;
	vec2 lightRadiusUV;
	float lightZNear;
	float lightZFar;
};


#if nDirLights > 0
uniform DirLight DirLights[nDirLights];
#endif

#if nPointLights > 0
uniform PointLight PointLights[nPointLights];
#endif

#if nSpotLights > 0
uniform SpotLight SpotLights[nSpotLights];
#endif

//ENUMS
#define LIGHT_DIRECTION 0
#define LIGHT_SPOT      1
#define LIGHT_POINT     2

const vec2 Poisson25[25] = vec2[](
    vec2(-0.978698, -0.0884121),
    vec2(-0.841121, 0.521165),
    vec2(-0.71746, -0.50322),
    vec2(-0.702933, 0.903134),
    vec2(-0.663198, 0.15482),
    vec2(-0.495102, -0.232887),
    vec2(-0.364238, -0.961791),
    vec2(-0.345866, -0.564379),
    vec2(-0.325663, 0.64037),
    vec2(-0.182714, 0.321329),
    vec2(-0.142613, -0.0227363),
    vec2(-0.0564287, -0.36729),
    vec2(-0.0185858, 0.918882),
    vec2(0.0381787, -0.728996),
    vec2(0.16599, 0.093112),
    vec2(0.253639, 0.719535),
    vec2(0.369549, -0.655019),
    vec2(0.423627, 0.429975),
    vec2(0.530747, -0.364971),
    vec2(0.566027, -0.940489),
    vec2(0.639332, 0.0284127),
    vec2(0.652089, 0.669668),
    vec2(0.773797, 0.345012),
    vec2(0.968871, 0.840449),
    vec2(0.991882, -0.657338)
);

float borderDepthTexture(sampler2D tex, vec2 uv)
{
	return 0.0f;
//	return ((uv.x <= 1.0) && (uv.y <= 1.0) &&
//	   (uv.x >= 0.0) && (uv.y >= 0.0)) ? textureLod(tex, uv, 0.0).z : 1.0;
}

float borderPCFTexture(int lightType, int lightID, vec3 uvz)
{
    float a = 0;
#if nDirLights > 0
    if(lightType == LIGHT_DIRECTION) {
        a = texture(DirLights[lightID].shadowMap, uvz.xy).x > uvz.z? 1.0:0.0;
    }
#endif
#if nSpotLights > 0
    if(lightType == LIGHT_SPOT) {
        a = texture(SpotLights[lightID].shadowMap, uvz.xy).x > uvz.z? 1.0:0.0;
    }
#endif

	return ((uvz.x <= 1.0) && (uvz.y <= 1.0) &&
	 (uvz.x >= 0.0) && (uvz.y >= 0.0)) ? a : 
	 ((uvz.z <= 1.0) ? 1.0 : 0.0);

}

// Using similar triangles from the surface point to the area light
vec2 searchRegionRadiusUV(float zWorld, int lightID, int lightType )
{
    vec2 rRadiusUV = vec2(0.0, 0.0);
#if nDirLights > 0
    if(lightType == LIGHT_DIRECTION)
        rRadiusUV = DirLights[lightID].lightRadiusUV * (zWorld - DirLights[lightID].lightZNear) / zWorld;
#endif
#if nSpotLights > 0
    if(lightType == LIGHT_SPOT)
        rRadiusUV = SpotLights[lightID].lightRadiusUV * (zWorld - SpotLights[lightID].lightZNear) / zWorld;
#endif
    return rRadiusUV;
}

// Using similar triangles between the area light, the blocking plane and the surface point
vec2 penumbraRadiusUV(float zReceiver, float zBlocker, int lightID, int lightType )
{
    vec2 pRadiusUV = vec2(0.0, 0.0);
#if nDirLights > 0
    if(lightType == LIGHT_DIRECTION)
        pRadiusUV = DirLights[lightID].lightRadiusUV * (zReceiver - zBlocker) / zBlocker;
#endif
#if nSpotLights > 0
    if(lightType == LIGHT_SPOT)
       pRadiusUV = SpotLights[lightID].lightRadiusUV * (zReceiver - zBlocker) / zBlocker;
#endif
    return pRadiusUV;
}

// Project UV size to the near plane of the light
vec2 projectToLightUV(vec2 sizeUV, float zWorld, int lightID, int lightType )
{
    vec2 pluv = vec2(0.0, 0.0);
#if nDirLights > 0
    if(lightType == LIGHT_DIRECTION)    
        pluv = sizeUV * DirLights[lightID].lightZNear / zWorld;
#endif
#if nSpotLights > 0
    if(lightType == LIGHT_SPOT)
        pluv = sizeUV * SpotLights[lightID].lightZNear / zWorld;
#endif
    return pluv;
}

// Derivatives of light-space depth with respect to texture2D coordinates
vec2 depthGradient(vec2 uv, float z)
{
    vec2 dz_duv = vec2(0.0, 0.0);

    vec3 duvdist_dx = dFdx(vec3(uv,z));
    vec3 duvdist_dy = dFdy(vec3(uv,z));


    dz_duv.x = duvdist_dy.y * duvdist_dx.z;
    dz_duv.x -= duvdist_dx.y * duvdist_dy.z;

    dz_duv.y = duvdist_dx.x * duvdist_dy.z;
    dz_duv.y -= duvdist_dy.x * duvdist_dx.z;

    float det = (duvdist_dx.x * duvdist_dy.y) - (duvdist_dx.y * duvdist_dy.x);
    dz_duv /= det;

    return dz_duv;
}

float biasedZ(float z0, vec2 dz_duv, vec2 offset)
{
    return z0 + dot(dz_duv, offset);
}

float getZClip(float zFar, float zNear, float z)
{
	return zFar * zNear / (zFar - z * (zFar - zNear));
}

float zClipToEye(float z, int lightID, int lightType)
{
    float zclip = 0.0;
#if nDirLights > 0
    if(lightType == LIGHT_DIRECTION)
        zclip = getZClip(DirLights[lightID].lightZFar, DirLights[lightID].lightZNear, z);
#endif
#if nSpotLights > 0
    if(lightType == LIGHT_SPOT)
        zclip = getZClip(SpotLights[lightID].lightZFar, SpotLights[lightID].lightZNear, z);
#endif
    return zclip;
}

// Returns average blocker depth in the search region, as well as the number of found blockers.
// Blockers are defined as shadow-map samples between the surface point and the light.
void findBlocker(
    out float accumBlockerDepth, 
    out float numBlockers,
    out float maxBlockers,
    vec2 uv,
    float z0,
    vec2 dz_duv,
    vec2 searchRegionRadiusUV,
    int lightID,
    int lightType)
{
    accumBlockerDepth = 0.0;
    numBlockers = 0.0;
    maxBlockers = 25.0;
    for (int i = 0; i < 25; ++i)
    {
        vec2 offset = Poisson25[i] * searchRegionRadiusUV;
        float shadowMapDepth = 0.0;
#if nDirLights > 0
        if(lightType == LIGHT_DIRECTION)
            shadowMapDepth = borderDepthTexture(DirLights[lightID].shadowMap, uv + offset);
#endif
#if nSpotLights > 0
        if(lightType == LIGHT_SPOT)
           shadowMapDepth = borderDepthTexture(SpotLights[lightID].shadowMap, uv + offset);
#endif
        float z = biasedZ(z0, dz_duv, offset);
        if (shadowMapDepth < z)
        {
            accumBlockerDepth += shadowMapDepth;
            numBlockers++;
        }
    }

}

// Performs PCF filtering on the shadow map using multiple taps in the filter region.
float pcfFilter(vec2 uv, float z0, vec2 dz_duv, vec2 filterRadiusUV, int lightID, int lightType)
{
    float sum = 0.0;

    for (int i = 0; i < 25; ++i)
    {
        vec2 offset = Poisson25[i] * filterRadiusUV;
        float z = biasedZ(z0, dz_duv, offset);
#if nDirLights > 0
        if(lightType == LIGHT_DIRECTION)
            sum += borderPCFTexture(lightType, lightID, vec3(uv + offset, z));
#endif
#if nSpotLights > 0
        if(lightType == LIGHT_SPOT)
           sum += borderPCFTexture(lightType, lightID, vec3(uv + offset, z));
#endif
    }
    return sum / 25.0;

}

float pcssShadow(vec2 uv, float z, vec2 dz_duv, float zEye, int lightID, int lightType)
{
    // ------------------------
    // STEP 1: blocker search
    // ------------------------
    float accumBlockerDepth, numBlockers, maxBlockers;
    vec2 searchRegionRadiusUV = searchRegionRadiusUV(zEye,lightID, lightType);
    findBlocker(accumBlockerDepth, numBlockers, maxBlockers, uv, z, dz_duv, searchRegionRadiusUV,lightID, lightType);

    // Early out if not in the penumbra
    if (numBlockers == 0.0)
        return 1.0;

    // ------------------------
    // STEP 2: penumbra size
    // ------------------------
    float avgBlockerDepth = accumBlockerDepth / numBlockers;
    float avgBlockerDepthWorld = zClipToEye(avgBlockerDepth,lightID, lightType);
    vec2 penumbraRadius = penumbraRadiusUV(zEye, avgBlockerDepthWorld,lightID, lightType);
    vec2 filterRadius = projectToLightUV(penumbraRadius, zEye,lightID, lightType);

    // ------------------------
    // STEP 3: filtering
    // ------------------------
    return pcfFilter(uv, z, dz_duv, filterRadius,lightID, lightType);
}


// Forward declaration, ShadingModel is implemented by the material shader, e.g. phong.frag
vec4 ShadingModel(vec3 N, vec3 P, vec3 L, vec4 lightColor, float intensity, vec4 vertexColor, vec3 materialValue);

vec3 ambientModel(vec3 N, vec3 P, vec4 vertexColor, vec3 materialValue, vec3 localPos);

vec3 PostProcessing(vec3 color);

vec4 PreProcessing(vec3 N, vec3 P, vec4 vertexColor, vec3 materialValue, vec3 localPos);

float calculateAttenuation(in float dist, in float attLin, in float attQuad)
{
	return (1.0 / (1.0 + attLin * dist + attQuad * dist * dist));
}

/*
	Directional Light
*/
#if nDirLights > 0
vec4 directionalLightVisibility(in int i, in vec4 vertexPosition)
{
    vec4 visibility = vec4(1, 1, 1, 1);
    if (isReceiveShadow &&  DirLights[i].castingShadow != 0)
    {
        vec4 ShadowCoord =  DirLights[i].DepthBiasMVP * vertexPosition;
        vec2 uv = ShadowCoord.xy / ShadowCoord.w;
        float z = ShadowCoord.z / ShadowCoord.w;

        if ( DirLights[i].castingShadow == 1)
        {
            if(uv.x >= 0.0 && uv.x <= 1.0 && uv.y >= 0.0 && uv.y <= 1.0 && z >= 0.0 && z <= 1.0){
                float shadowDepth = texture(  DirLights[i].shadowMap, uv ).z;
                if (shadowDepth < z){
                    visibility =  DirLights[i].shadowColor;
                }
            }
        }
        else if( DirLights[i].castingShadow == 2)
        {
            vec2 dz_duv = depthGradient(uv, z);

            // Eye-space z from the light's point of view
            float zEye = -( DirLights[i].lightView * vertexPosition).z;
            float shadow = pcssShadow(uv, z, dz_duv, zEye, i, LIGHT_DIRECTION);
            visibility =  DirLights[i].shadowColor * (1.0 - shadow) + vec4(1) * shadow;
        }
    }
    return visibility;
}

vec4 directionalLight(in int i, in vec3 N, in vec3 P, in vec4 vertexColor, in vec4 worldPos, in vec3 materialValue)
{
	if (!DirLights[i].isactive) return vec4(0);
	vec3 L = -DirLights[i].dir;
	L = normalize(L);
	float nDotL = dot(N, L);

//	float bias = 0.002*tan(acos(nDotL)); // cosTheta is dot( n,l ), clamped between 0 and 1
//	bias = clamp(bias, 0.0, 0.005); 

	vec4 visibility = vec4(1);
	if(nDotL > 0.1){
		visibility = directionalLightVisibility(i, worldPos);
	}	

	return  visibility*ShadingModel(N, P, L, DirLights[i].color, DirLights[i].intensity, vertexColor, materialValue);
}
#endif

int getNrOfDirLights()
{
	return nDirLights;
}

vec3 DirLightDirection(int i)
{
#if nDirLights > 0
	return DirLights[i].dir;
#else
	return vec3(0);
#endif
}

vec4 DirLightColor(int i)
{
#if nDirLights > 0
	return DirLights[i].color;
#else
	return vec4(0);
#endif
}

float DirLightIntensity(int i)
{
#if nDirLights > 0
	return DirLights[i].intensity;
#else
	return float(0);
#endif
}
/*
	Point Light
*/
#if nPointLights > 0

vec4 pointLight(in int i, in vec3 N, in vec3 P, in vec4 vertexColor, in vec4 worldPos, in vec3 materialValue)
{
	if (!PointLights[i].isactive) return vec4(0);
	vec3 D = PointLights[i].pos - P;
	vec3 L = normalize(D);

	float dist = length(D);
	float attenuation = calculateAttenuation(dist, PointLights[i].attLin, PointLights[i].attQuad);
	return attenuation * ShadingModel(N, P, L, PointLights[i].color, PointLights[i].intensity, vertexColor, materialValue);
}
#endif

/*
	Spot Light
*/
#if nSpotLights > 0

vec4 spotLightVisibility(in int i, in vec4 vertexPosition)
{
    vec4 visibility = vec4(1, 1, 1, 1);
    if (isReceiveShadow &&  SpotLights[i].castingShadow != 0)
    {
        vec4 ShadowCoord =  SpotLights[i].DepthBiasMVP * vertexPosition;
        vec2 uv = ShadowCoord.xy / ShadowCoord.w;
        float z = ShadowCoord.z / ShadowCoord.w;

        if ( SpotLights[i].castingShadow == 1)
        {
            if(uv.x >= 0.0 && uv.x <= 1.0 && uv.y >= 0.0 && uv.y <= 1.0 && z >= 0.0 && z <= 1.0){
                float shadowDepth = texture(  SpotLights[i].shadowMap, uv ).z;
                if (shadowDepth < z){
                    visibility =  SpotLights[i].shadowColor;
                }
            }
        }
        else if( SpotLights[i].castingShadow == 2)
        {
            vec2 dz_duv = depthGradient(uv, z);

            // Eye-space z from the light's point of view
            float zEye = -( SpotLights[i].lightView * vertexPosition).z;
            float shadow = pcssShadow(uv, z, dz_duv, zEye, i, LIGHT_SPOT);
            visibility =  SpotLights[i].shadowColor * (1.0 - shadow) + vec4(1) * shadow;
        }
    }
    return visibility;
}

vec4 spotLight(in int i, in vec3 N, in vec3 P, in vec4 vertexColor, in vec4 worldPos, in vec3 materialValue)
{
	if (!SpotLights[i].isactive) return vec4(0);
	vec3 L = -SpotLights[i].dir;
	L = normalize(L);
	float nDotL = dot(N, L);

//	float bias = 0.002*tan(acos(nDotL)); // cosTheta is dot( n,l ), clamped between 0 and 1
//	bias = clamp(bias, 0.0, 0.005);

	vec4 visibility = vec4(1);
	if(nDotL > 0.1){
		visibility = spotLightVisibility(i, worldPos);
	}
	
	vec3 D = SpotLights[i].pos - P;
	L = normalize(D);
	vec4 black = vec4(0.0, 0.0, 0.0, 1.0);

	vec4 tmpColor = ShadingModel(N, P, L, SpotLights[i].color*3, SpotLights[i].intensity, vertexColor, materialValue);

	if(tmpColor.xyz != black.xyz)
	{
		float spotEffect = dot(SpotLights[i].dir, -L);

		if(spotEffect > SpotLights[i].cosCutoff)
		{
			float dist = length(D);
			float attenuation = calculateAttenuation(dist, SpotLights[i].attLin, SpotLights[i].attQuad);
			attenuation *= pow(max(spotEffect, 0.01f), SpotLights[i].exponent);
			return visibility*attenuation * tmpColor;
		}
	}
	return black;
}
#endif


vec4 calculateLighting(in vec3 N, in vec3 P, in vec4 vertexColor, in vec4 worldPos, in vec3 materialValue, in vec3 localPos)
{
	vec4 color = vec4(0.0);
	int i;

	vertexColor = PreProcessing(N, P, vertexColor, materialValue, localPos);
	
#if nDirLights > 0
	for(i = 0; i < nDirLights; ++i)
		color += directionalLight(i, N, P, vertexColor, worldPos, materialValue);
#endif
#if nPointLights > 0
	for(i = 0; i < nPointLights; ++i)
		color += pointLight(i, N, P, vertexColor, worldPos, materialValue);
#endif
#if nSpotLights > 0
	for(i = 0; i < nSpotLights; ++i)
		color += spotLight(i, N, P, vertexColor, worldPos, materialValue);
#endif

    color += vec4(ambientModel(N, P, vertexColor, materialValue, localPos), 1);

	color.rgb = PostProcessing(color.rgb);

	color = clamp(color, 0.0, 1.0);
	return color;
}


vec4 calculateVisibility(in vec4 vertexPosition)
{
	vec4 visble = vec4(0.0);
	int i;

	
#if nDirLights > 0
	for(i = 0; i < nDirLights; ++i)
		visble += directionalLightVisibility(i, vertexPosition);
#endif
#if nPointLights > 0
	for(i = 0; i < nPointLights; ++i)
		visble += vec4(1,1,1,1);
#endif
#if nSpotLights > 0
	for(i = 0; i < nSpotLights; ++i)
		visble += spotLightVisibility(i, vertexPosition);
#endif
	visble = clamp(visble, 0.0, 1.0);
	return visble;
}