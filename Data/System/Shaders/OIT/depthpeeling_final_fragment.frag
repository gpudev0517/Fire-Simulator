#version 130

out vec4 outColor;

in vec2 texCoord;

uniform sampler2D ColorTex;
uniform sampler2D bgTex;

void main(void)
{
    vec4 frontColor = texture(ColorTex, texCoord);
    vec4 bgColor = texture(bgTex, texCoord);
    float transmittance = frontColor.a;

    outColor.rgb = frontColor.rgb + bgColor.rgb * transmittance;
}