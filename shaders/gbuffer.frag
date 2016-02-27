#version 330
//#version 420

//out variables
layout(location = 0) out vec4 cubeMapW0;
layout(location = 1) out vec4 cubeMapW1;

//uniform variables 
uniform bool firstPass;			//indicates the first pass of the depth peeling (layer-0)

uniform sampler2D texSampler;	//the texture to the model
//layout(binding = 0) uniform samplerCube cubeMapR0;	//the previous layer on the depth peeling (layer-k)
uniform samplerCube cubeMapR0;	//the previous layer on the depth peeling (layer-k)
//uniform mat4 mLookAt, mProjection;
uniform int bSpecularOject;
uniform int bHasTexture;

in vec2 vVertexUV;
in vec4 vVertexColor;
in vec3 N;
in vec4 vertexWC;	//vertex on world coordinates
in vec4 eyeWC;			//eye on world coordinates

vec3 getColor()
{
	if(bHasTexture == 0)
		return vVertexColor.rgb;
	else
		return texture(texSampler, vVertexUV).rgb;
}

//float EPSILON = 0.001;
float EPSILON = 0.001;

// cubeMapW0.rgba = {color.xyz, specular?}
// cubeMapW1.rgba = {normal.xyz, distance}
void main(void)
{
	float distWC = distance(vertexWC.xyz, eyeWC.xyz);
	//float distWC = distance(vertexWC.xyz, vec3(0));
	vec3 vectorDir = normalize(vertexWC.xyz - eyeWC.xyz);	//always is 1
	//vec3 vectorDir = normalize(vertexWC.xyz - vec3(0));
	float dist2PrevLayer = 0;
	
	if(firstPass)
	{
		cubeMapW0.rgb = getColor();
		cubeMapW0.a = bSpecularOject;
		cubeMapW1.xyz = normalize(N);
		cubeMapW1.a = distWC;
	}
	else	//from the 2nd layer
	{
		//cubeMapW0.rgb = getColor();
		//cubeMapW0.a = bSpecularOject;
		//cubeMapW1.xyz = normalize(N);
		//cubeMapW1.a = distWC;

		//mat3x3 mRot = mat3x3(transpose(inverse(mLookAt)));
		//vec3 rotateDir = mRot * vectorDir;
		////cubeMapW0.rgb = texture(cubeMapR0, vectorDir).aaa;
		////cubeMapW0.a = distWC;
		////cubeMapW1.xyz = normalize(N);
		////cubeMapW1.a = distWC;
		//dist2PrevLayer = texture(cubeMapR0, vectorDir).a;
		cubeMapW0.rgb = texture(cubeMapR0, vectorDir).rgb;
		cubeMapW0.a = 1;
		cubeMapW1.xyz = normalize(N);
		cubeMapW1.a = distWC;
		////if (distWC < (dist2PrevLayer + EPSILON))
		////{
		////	discard;
		////}
		////else
		////{
		////	cubeMapW0.rgb = getColor();
		//////	//cubeMapW0.rgb = vectorDir;
		////	cubeMapW0.a = distWC;
		////	cubeMapW1.xyz = normalize(N);
		////	cubeMapW1.a = distWC;
		////}
	}
}