#version 120

/*
 * Developer : Alexander Dippel (adippel@informatik.uni-freiburg.e)
 * Date : 04/10/2012
 * 
 * This shader filters shadow values produced by SSAO. 
 * A gaußian filter is applied to achieve this.
 * In the end, the shadows and intensities are merged with a pre-rendered
 * backround image.
 * 
 */

uniform float nearClip;
uniform float farClip;
// Screen width
uniform float width;
// Screen height
uniform float height;

uniform vec3 camPos;

 // Point size in world space
uniform float pointRadius; 
uniform float filterRadius;
// The gauß filter's variance
uniform float variance;

uniform vec4 foamColor;
uniform float irradianceWeight;
uniform float irradianceScale;
uniform float specularWeight;
uniform float specularScale;
uniform vec4 shadowColor;

uniform sampler2D tex0;
uniform sampler2D tex1;
// Per fragment world space positions
uniform sampler2D tex3;
// Background picture
uniform sampler2D tex4;
// Environment picture
uniform sampler2D tex5;
// Fragment normal and type
uniform sampler2D tex6;
// Fragment radiance
uniform sampler2D tex7;


const float PI = 3.141592654;
const float InvPI = 1.0/PI;
const float Inv2PI = 1.0/(2.0*PI);
/*
float fresnel(float cosThetaAbs, float n)
{
	float rf0 = (n-1)/(n+1);
	rf0 = rf0*rf0;

	float a = (1.0-cosThetaAbs);
	float ae = a*a;
	ae = ae*ae;
	ae*=a;

	return rf0 + (1.0-rf0)*ae;
}
*/

// Main function
void main()
{
	vec2 screenCoord = gl_FragCoord.xy;
	float invWidth = 1.0 / width;
	float invHeight = 1.0 / height;

	screenCoord.x *= invWidth;
	screenCoord.y *= invHeight;
	

	vec4 tex0Val = texture2D(tex0, screenCoord);
	vec4 tex1Val = texture2D(tex1, screenCoord);
	/*vec4 tex6Val = texture2D(tex6, screenCoord);

	vec3 worldPos = texture2D(tex3, screenCoord);

	vec3 eyeVector = worldPos-camPos;
	eyeVector = normalize(eyeVector);

	vec3 normal = tex6Val.xyz;

	vec3 reflVector = reflect(eyeVector, normal);

	float fragType = tex6Val.w;
*/

	float intensity = tex1Val.y;
	float rigidEyeDepth = tex0Val.y;
	float foamEyeDepth = tex0Val.z;

	
	// Get the background image's color for this fragment
	vec4 color = texture2D(tex4, screenCoord);
	// Set this line to disable the background
	//color = vec4(0.0, 0.0, 0.0, 0.0);

	vec3 radiance = vec3(0,0,0);
	
	// Case: Fragment is on foam and no rigid occludes it
	if(foamEyeDepth > 0.0 && foamEyeDepth <= rigidEyeDepth)
	{
// Fresnel reflection related part
/*		float theta = acos(reflVector.y);
		float phi = atan(reflVector.z, reflVector.x);
					
		if (phi < 0) phi += 2*PI;
		if (phi > 2*PI) phi -= 2*PI;

		vec4 reflectedEnvColor = texture2D(tex5, vec2( phi / (2.0*PI), 1.0 - theta / PI ));
		reflectedEnvColor *= specularScale;

		reflectedEnvColor.x = clamp(reflectedEnvColor.x, 0, 1);
		reflectedEnvColor.y = clamp(reflectedEnvColor.y, 0, 1);
		reflectedEnvColor.z = clamp(reflectedEnvColor.z, 0, 1);

		float cosTheta = abs(dot(-eyeVector, normal));
		float fresnelTerm = fresnel(cosTheta, 1.33);

		float wSum = irradianceWeight + specularWeight;
		float irrWeight = irradianceWeight;
		float speWeight = specularWeight;

		if(fresnelTerm > wSum)
		{
			irrWeight = 0;
			speWeight = wSum;
		}
		else if(fresnelTerm > specularWeight)
		{
			irrWeight = wSum - fresnelTerm;
			speWeight = fresnelTerm;
		}
		else
			speWeight = fresnelTerm;

*/
		vec3 realShadowColor = vec3(1.0,1.0,1.0) - shadowColor.xyz;

		vec3 shadow = realShadowColor*tex1Val.x;
		radiance = texture2D(tex7, screenCoord).xyz;
		radiance *= irradianceScale;
		radiance.x = clamp(radiance.x, 0.0, 1.0);
		radiance.y = clamp(radiance.y, 0.0, 1.0);
		radiance.z = clamp(radiance.z, 0.0, 1.0);


		vec2 screenCoord2;
		
		float searchRadiusPixel = filterRadius;
		searchRadiusPixel = max(tex0Val.w*width*0.5*filterRadius, 2.0);
		if(filterRadius == 0.0)
			searchRadiusPixel = 0.0;
			
		if(searchRadiusPixel > 0.0)
		{
			// Apply a gaussian filter to the shadow
			float h = 0.0, x = 0.0, y = 0.0;
			// The extend is used to shift the function so that it hits 0 at the borders of the search radius
			float ext = 1.0/(2.0*PI*variance*exp((2.0)/(2.0*variance)));
			float volume = 0.0;

			shadow = vec3(0,0,0);
			radiance = vec3(0,0,0);

			float invSearchRadiusPixel = 1.0 / searchRadiusPixel;
			float inv2Variance = 1.0/(2.0*variance);

			// Iterate over all neighboring fragments within the search radius
			for(float i = -searchRadiusPixel; i <= searchRadiusPixel; i += 1.0)
			{
				for(float j = -searchRadiusPixel; j <= searchRadiusPixel; j += 1.0)
				{
					x = i*invSearchRadiusPixel;
					y = j*invSearchRadiusPixel;
					
					if(sqrt(x*x + y*y) <= 1.0)
					{				
						screenCoord2.x = gl_FragCoord.x + i;
						screenCoord2.y = gl_FragCoord.y + j;
						
						if(screenCoord2.x > 0.0 && screenCoord2.x < width && screenCoord2.y > 0.0 && screenCoord2.y < height)
						{
							screenCoord2.x *= invWidth;
							screenCoord2.y *= invHeight;

							vec4 tex0Val2 = texture2D(tex0, screenCoord2);
							vec4 tex1Val2 = texture2D(tex1, screenCoord2);

							//vec3 radiance = texture2D(tex7, screenCoord2).xyz;

							//radiance.x = clamp(radiance.x, 0, 1);
							//radiance.y = clamp(radiance.y, 0, 1);
							//radiance.z = clamp(radiance.z, 0, 1);

							//radiance = vec3(1,1,1) - radiance;
							
							// Only contribute if there is foam that is not occluded by a rigid
							if(tex1Val2.y > 0.0 && tex0Val2.z < tex0Val2.y)
							{
								shadow += realShadowColor*h*tex1Val2.x;
								radiance += h*texture2D(tex7, screenCoord2).xyz;
								h = 1.0/(2.0*PI*variance*exp((x*x + y*y)*inv2Variance)) - ext;
								volume += h;
							}
						}
					}
				}
			}
			if(volume > 0.0)
			{
				shadow /= volume;
				radiance /= volume;
				radiance *= irradianceScale;
				radiance.x = clamp(radiance.x, 0.0, 1.0);
				radiance.y = clamp(radiance.y, 0.0, 1.0);
				radiance.z = clamp(radiance.z, 0.0, 1.0);
			}
		}

		//shadow = (1.0-irrWeight-speWeight)*vec3(1,1,1) + irrWeight*radiance + speWeight*reflectedEnvColor - shadow;
		shadow = (1.0-irradianceWeight)*vec3(1.0,1.0,1.0) + irradianceWeight*radiance - shadow;
		//shadow = reflectedEnvColor;

		// Calculate final color
		// Source - destination blending is used:
		// The background color is the destination, the intensity the alpha value and the shadow the source color (gray)
		color = vec4(color.x*(1.0 - intensity) + foamColor.x*shadow.x*intensity,
					color.y*(1.0 - intensity) + foamColor.y*shadow.y*intensity,
					color.z*(1.0 - intensity) + foamColor.z*shadow.z*intensity, 1.0);
	}
	// Filter rigid shadows
	/*else if(rigidEyeDepth < 1.0 && foamEyeDepth > rigidEyeDepth)
	{
		vec2 screenCoord2;
		
		float searchRadiusPixel = filterRadius;
		if(filterRadius == 0.0)
			searchRadiusPixel = 0.0;
			
		// Apply a gaussian filter to the shadow
		float h, sigma = 0.35, PI = 3.141592654, x, y;
		float ext = 1.0/(2.0*PI*sigma*sigma*exp((2.0)/(2.0*sigma*sigma)));
		float volume = 0.0;
		for(float i = -searchRadiusPixel; i <= searchRadiusPixel; i += 1.0)
		{
			for(float j = -searchRadiusPixel; j <= searchRadiusPixel; j += 1.0)
			{	
				x = i/searchRadiusPixel;
				y = j/searchRadiusPixel;
				
				if(sqrt(x*x + y*y) <= 1.0)
				{
					screenCoord2.x = gl_FragCoord.x + i;
					screenCoord2.y = gl_FragCoord.y + j;
					
					if(screenCoord2.x > 0.0 && screenCoord2.x < width && screenCoord2.y > 0.0 && screenCoord2.y < height)
					{
						screenCoord2.x /= width;
						screenCoord2.y /= height;
						if(texture2D(tex5, screenCoord2).y > 0.0 && texture2D(tex5, screenCoord2).z > texture2D(tex5, screenCoord2).w)
						{
							
								h = 1.0/(2.0*PI*sigma*sigma*exp((x*x + y*y)/(2.0*sigma*sigma))) - ext;
								volume += h;
								shadow += h*texture2D(tex5, screenCoord2).x;
						}
					}
				}
			}
		}
		if(volume > 0.0)
			shadow /= volume;
		
		// Calculate final color
		// For rigids the shadow value is simply multiplied to the background color darkening it.
		color *= vec4(shadow, shadow, shadow, 1.0);
	}*/


	//if(fragType < 1.5/16.0 && fragType > 0.5/16.0)	// If there is a bubble fragment
	//{
	//	float weight = 0.5;
//		color *= color*(1.0-weight) + envColor*weight;
		//color *= (envColor * rainbowColor);
	//}
	
	
	gl_FragData[0] = color;

	//gl_FragData[1] = vec4(radiance, 1.0);//texture2D(tex5, screenCoord);

	//gl_FragData[1] = vec4(reflectedEnvColor);//texture2D(tex5, screenCoord);
	return;
}
