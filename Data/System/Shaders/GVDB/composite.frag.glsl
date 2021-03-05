#version 130

in vec2 texCoord;

out vec4 FragColor;

// simple shader that renders a texture without lighting
uniform sampler2D uTex1;
uniform sampler2D uTex2;

void main()
{
	vec4 op1 = texture2D( uTex1, vec2(texCoord.x, texCoord.y) );
	vec4 op2 = texture2D( uTex2, vec2(texCoord.x, texCoord.y) );
	FragColor = vec4( op1.xyz*(1.0-op2.w) + op2.xyz * op2.w, 1 );
	//FragColor = op2;
}