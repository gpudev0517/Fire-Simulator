#version 120

// Screen width
uniform float width;
// Screen height
uniform float height;

// Foam search radius (at .w)
uniform sampler2D tex0;
// Modified intensities (at .w)
uniform sampler2D tex2;
// Per fragment normals
uniform sampler2D tex5;

const float PI = 3.141592654;
const float InvPI = 1.0/PI;
const float Inv2PI = 1.0/(2.0*PI);

const float variance = 0.5;

// Main function
void main()
{	
	vec2 screenCoord = vec2(gl_FragCoord.x/width, gl_FragCoord.y/height);
	float invWidth = 1.0 / width;
	float invHeight = 1.0 / height;

	float radius = texture2D(tex0, screenCoord).w;
	float intensity = texture2D(tex2, screenCoord).w;
	vec3 normal = texture2D(tex5, screenCoord).xyz;

	vec3 smoothNormal = vec3(0,0,0);


	// If there is a visible fragment
	if(intensity > 0.0)
	{
		smoothNormal = normal;
		
		vec2 screenCoord2;
		
		//float searchRadiusPixel = filterRadius;
		float searchRadiusPixel = max(radius*width*0.5, 2.0);
		
		if(radius == 0.0)
			searchRadiusPixel = 0.0;

		smoothNormal = vec3(0,0,0);
			
		if(searchRadiusPixel > 0.0)
		{
			// Apply a gaussian filter to the shadow
			float h = 0.0, x = 0.0, y = 0.0;
			// The extend is used to shift the function so that it hits 0 at the borders of the search radius
			float ext = 1.0/(2.0*PI*variance*exp((2.0)/(2.0*variance)));
			float volume = 0.0;
			//float variance = 0.5;


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

							vec3 normalSample = texture2D(tex5, screenCoord2).xyz;
							float intensitySample = texture2D(tex2, screenCoord2).w;

						
							// Only contribute if there is foam that is not occluded by a rigid
							if(intensitySample > 0.0)
							{
								smoothNormal += /*h**/normalSample;
								//h = 1.0/(2.0*PI*variance*exp((x*x + y*y)*inv2Variance)) - ext;
								//volume += h;
							}
						}
					}
				}
			}
		}
	}
	gl_FragData[0] = vec4(normalize(smoothNormal), 0);
	return;
}
