#version 130

uniform sampler2D depthTex;
uniform float blurRadiusWorld;
uniform float blurScale;
uniform vec2 invTexScale;

uniform bool debug;

in vec2 texCoord;

// fluid surface setting
uniform float maxRadialSamples = 5.0f;
uniform float depthEdgeFalloff = 0.01f;
uniform float depthBlurFalloff = 5.5f;

float sqr(float x) { return x*x; }

void main()
{
    // eye-space depth of center sample
    float depth = texture2D(depthTex, texCoord).x;

	if (debug)
	{
		// do not blur
		gl_FragColor.x = depth;
		return;
	}

	// threshold on thickness to create nice smooth silhouettes
	if (depth == 0.0)
	{
		gl_FragColor.x = 0.0;
		return;
	}
	
	//discontinuities between different tap counts are visible. to avoid this we 
	//use fractional contributions between #taps = ceil(radius) and floor(radius) 
	float radius = min(maxRadialSamples, blurScale * (blurRadiusWorld / -depth));
	float radiusInv = 1.0/radius;
	float taps = ceil(radius);
	float frac = taps - radius;

	radiusInv = depthEdgeFalloff;

	float sum = 0.0;
    float wsum = 0.0;
	float count = 0.0;

    for(float y=-taps; y <= taps; y += 1.0)
	{
        for(float x=-taps; x <= taps; x += 1.0)
		{
			vec2 offset = vec2(x, y);

            float fSample = texture2D(depthTex, texCoord + offset*invTexScale).x;

			//if (fSample < -10000.0*0.5)
			if (fSample == 0)
				continue;

            // spatial domain
            float r1 = length(vec2(x, y))*radiusInv;
			float w = exp(-(r1*r1));

			//float expectedDepth = depth + dot(vec2(dzdx, dzdy), offset);

            // range domain (based on depth difference)
            float r2 = (fSample - depth) * depthBlurFalloff;
            float g = exp(-(r2*r2));

			//fractional radius contributions
			float wBoundary = step(radius, max(abs(x), abs(y)));
			float wFrac = 1.0 - wBoundary*frac;

			sum += fSample * w * g * wFrac;
			wsum += w * g * wFrac;
			count += g * wFrac;
        }
    }

    if (wsum > 0.0) {
        sum /= wsum;
    }

	float blend = count/sqr(2.0*radius+1.0);
	gl_FragColor.x = mix(depth, sum, blend);
}