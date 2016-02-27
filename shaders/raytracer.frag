#version 330

layout(location = 0) out vec4 vFragColor;

uniform samplerCube sSamplerCube0;
uniform samplerCube sSamplerCube1;
uniform samplerCube sSamplerCube2;
uniform samplerCube sSamplerCube3;
uniform samplerCube sSamplerCube4;
uniform samplerCube sSamplerCube5;

in vec2 vVertexUV;

void main()
{
	vec4 buffer1FromTex = texture(sSamplerCube0, vec3(vVertexUV, -1));
	vec4 buffer2FromTex = texture(sSamplerCube1, vec3(vVertexUV, -1));
	vFragColor = buffer1FromTex;
}