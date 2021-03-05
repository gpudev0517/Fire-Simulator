#version 130

in vec2 texCoord;
out vec4 FragColor;

uniform vec3 meshColor1;
uniform vec3 meshColor2;

uniform int bRadial = 0;

void main() 
{
	if(bRadial == 0){
		FragColor.rgb = mix( meshColor1, meshColor2, 1.0 - texCoord.y );
	}
	else{
		
		vec2 vDir = texCoord - vec2(0.5, 0.5);
		
		float dist = sqrt(vDir.x * vDir.x + vDir.y * vDir.y);
		
		FragColor.rgb = mix( 0.9 * meshColor1, meshColor2, dist * sqrt(2.0));
	}
	FragColor.a = 1.0;
}