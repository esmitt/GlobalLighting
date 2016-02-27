#version 330

//matrices uniform
//uniform struct Matrices
//{
//	mat4 mProjection;
//	mat4 mModelView;
//	mat4 mModel;
//} matrix;

uniform	mat4 mProjection;
uniform	mat4 mModelView;
uniform mat4 mModel;
uniform mat4 mView;
uniform mat4 mLookAt;


//uniform mat4 mProjection, mModelView, mModel;
uniform vec3 fCenterPosition;	//the center of the scene, the position of the camera
uniform mat3x3 mNormal;

//in variables
layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec4 vColor;
layout(location = 3) in vec2 vTexCoord;


out vec3 N;
out vec4 vertexWC;	//vertex on world coordinates
out vec4 eyeWC;		//eye on world coordinates
out vec4 vVertexColor;
out vec2 vVertexUV;

void main()
{
	//vertexWC = mLookAt * mModel * vVertex;
	vertexWC = mModel * vVertex;
	//eyeWC = inverse(mView * mModel) * vec4(fCenterPosition, 1);
	//eyeWC = mLookAt * mModel * vec4(fCenterPosition, 1);
	//eyeWC = vec4(vec3(0,0,0), 1);
	//eyeWC = inverse(mModel) * vec4(fCenterPosition, 1);
	eyeWC = inverse(mModel) * vec4(fCenterPosition, 1);
	//eyeWC = mModel * vec4(fCenterPosition, 1);
	//eyeWC.xyz /= eyeWC.w;
	//eyeWC = vec4(fCenterPosition, 1);
	//eyeWC = vec4(0,0,0,1);

	N = mNormal * vNormal;
	vVertexColor = vColor;
	vVertexUV = vTexCoord;
	//gl_Position = mProjection * mModelView * vVertex;
	gl_Position = mProjection * mView * mModel * vVertex;
	//gl_Position = matrix.mProjection * matrix.mModel * vVertex;
}