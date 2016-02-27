#version 400

uniform mat4 mProjection, mModelView;

layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec4 vNormal;
layout(location = 2) in vec4 vColor;
layout(location = 3) in vec2 vTexCoord;

out vec4 vVertexColor;
out vec2 vVertexUV;

void main()
{
	vVertexColor = vColor;
	vVertexUV = vTexCoord;
	gl_Position = mProjection * mModelView * vVertex;
}