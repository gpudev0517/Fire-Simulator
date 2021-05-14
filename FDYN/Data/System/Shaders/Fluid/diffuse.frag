#version 150

float sqr(float x) { return x*x; }
float cube(float x) { return x*x*x; }

uniform sampler2D depthTex;
uniform sampler2D noiseTex;
uniform vec2 invViewport;
uniform vec4 color;
uniform bool front;
uniform bool shadow;

//uniform sampler2DShadow shadowTex;
uniform sampler2D shadowTex;
uniform vec2 shadowTaps[12];
uniform mat4 lightTransform;
uniform vec3 lightDir;
uniform float inscatterCoefficient;
uniform float outscatterCoefficient;

in vec4 texCoord[6];

void main()
{
	float attenuation = texCoord[4].w;
	float lifeFade = min(1.0, texCoord[1].w*0.125);

    // calculate normal from texture coordinates
    vec3 normal;
    normal.xy = texCoord[0].xy*vec2(2.0, 2.0) + vec2(-1.0, -1.0);
    float mag = dot(normal.xy, normal.xy);
    if (mag > 1.0) discard;   // kill pixels outside circle
   	normal.z = 1.0-mag;

	float velocityFade = texCoord[3].w;
	float alpha = lifeFade*velocityFade*sqr(normal.z);

	gl_FragColor = vec4(vec3(alpha), 1);
}