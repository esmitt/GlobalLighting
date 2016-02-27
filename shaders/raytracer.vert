#version 330

layout(location = 0) in vec3 vVertex;
layout(location = 1) in vec2 vTexCoord;

out vec2 vVertexUV;

void main()
{
	vVertexUV = vTexCoord;
	gl_Position = vec4(vVertex, 1.0);
}