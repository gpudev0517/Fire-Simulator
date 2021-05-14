#version 130

in vec2 texCoord;

out vec4 FragColor;

// simple shader that renders a texture without lighting
uniform sampler2D colorMap;

void main()
{
	FragColor = texture2D( colorMap, texCoord );
}