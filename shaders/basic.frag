#version 400

in vec4 vVertexColor;
in vec2 vVertexUV;
uniform sampler2D sTexture;
uniform int bHasTexture;
layout(location = 0) out vec4 vFragColor;

void main(void)
{
	//if bHasTexture == 0 then vVertexColor else texture(sTexture, vVertexUV)
	vFragColor = mix(vVertexColor, texture(sTexture, vVertexUV), bHasTexture);
}