#version 140 

out vec4 outColor;

uniform sampler2DRect TempTex;

void main(void)
{
    outColor = texture(TempTex, gl_FragCoord.xy);
}