#version 400

in vec4 vVertexColor;
in vec2 vVertexUV;
uniform sampler2D sTexture;
uniform int bHasTexture;
layout(location = 0) out vec4 vFragColor;

void main(void)
{
	//vFragColor = vVertexColor;
	//vFragColor = texture(sTexture, vVertexUV);
	vFragColor = mix(vVertexColor, texture(sTexture, vVertexUV), bHasTexture);
	//vFragColor.a = .5 - vVertexUV.x;
}