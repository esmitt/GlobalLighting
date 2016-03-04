#include <GL/glew.h>	//this line is mandatory at first place!
#include <imdebug/imdebuggl.h>
#include <stdlib.h>
#include <iostream>
#include <limits.h>
#include "GBuffer.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#ifdef _WIN32
	#pragma comment(lib, "imdebug.lib")
#endif

//#define GL_CHECK_ERRORS2 assert(glGetError()== GL_NO_ERROR);
#define printOpenGLError2() printOglError2(__FILE__, __LINE__)
int printOglError2(char *file, int line)
{
    GLenum glErr;
    int    retCode = 0;
    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        ///std::cout << "glError in file " << file << "@ line " << line << ": " << gluErrorString(glErr) << std::endl;
			std::cout << "glError in file " << file << "@ line " << line << std::endl;
        retCode = 1;
        glErr = glGetError();
    }
    return retCode;
}

///
/// Simple constructor of the class
///
CGBuffer::CGBuffer(void)
{
	m_puCubeTexId = NULL;
	m_uFBOname = 0;
	m_uFBODepthName = 0;
	m_iNTextures = m_iNLayers = 0;
	m_puCubeTexId = NULL;
}

///
/// Simple destructor of the class
///
CGBuffer::~CGBuffer(void)
{
	if(m_uFBOname > 0)
		glDeleteFramebuffers(1, &m_uFBOname);
	if(m_uFBODepthName > 0)
		glDeleteFramebuffers(1, &m_uFBODepthName);
	if(m_puCubeTexId)
		glDeleteTextures(m_iNTextures, m_puCubeTexId);
}

///
/// Create the textures according to the number of layer passed
///
/// @param k indicates the number of layer to be created
///
bool CGBuffer::setNumberOfLayer(int k, int width, int height)
{
	std::cout << "number of layers =" << k << std::endl;
	if((k<<1) > MAX_NUMBER_OF_TEXTURES) return false;	//this value must be checked according the videocard specifications
	if(m_puCubeTexId)	//if exists
		delete [] m_puCubeTexId;
	m_iNLayers = k;
	m_iNTextures = k<<1;	//each texture x 2
	m_iWidth = width;
	m_iHeight = height;
	m_puCubeTexId = new unsigned int[m_iNTextures];

	std::cout << "gbuffer with " << m_iNTextures << " textures" << std::endl;
	glGenTextures(m_iNTextures, m_puCubeTexId);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	for(int l = 0; l < m_iNTextures; l++)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_puCubeTexId[l]);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		for (k= 0; k < 6; k++)				//six faces of the cubemap
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + k, 0, GL_RGBA32F,
			m_iWidth, m_iHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	}
	printOpenGLError2();

	//depth buffer
	glGenRenderbuffers(1, &m_uFBODepthName);
	glBindRenderbuffer(GL_RENDERBUFFER, m_uFBODepthName);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_iWidth, m_iHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	printOpenGLError2();

	//create the FB and the cubemaps textures
	glGenFramebuffers(1, &m_uFBOname);
	glBindFramebuffer(GL_FRAMEBUFFER, m_uFBOname);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_uFBODepthName);
	
	GLenum draws [2] ={GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};		//two textures by layer
	glDrawBuffers(2, draws);
	printOpenGLError2();
	
	//initialize at least layer-0 and layer-1
	//for (int k = 0; k < 6; k++)	//six faces of the cubemap
	//{
	//	glBindTexture(GL_TEXTURE_CUBE_MAP, m_puCubeTexId[0]);
	//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + k, m_puCubeTexId[0], 0);
	//	printOpenGLError4();
	//	glBindTexture(GL_TEXTURE_CUBE_MAP, m_puCubeTexId[1]);
	//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_CUBE_MAP_POSITIVE_X + k, m_puCubeTexId[1], 0);
	//}

	// attach only the +X cubemap texture (for completeness)
	//glBindTexture(GL_TEXTURE_CUBE_MAP, m_puCubeTexId[0]);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, m_puCubeTexId[0], 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) return false;
	//printOpenGLError4();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//printOpenGLError4();
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	printOpenGLError2();
	std::cout << "FBOs made correctly" << std::endl;
	return true;
}

///
/// This function initialize the g-buffer variables and all the parameters.
/// Is mandatory call it before using others functions in the class.
/// @param iNumLayer indicates the number of layer of the gbuffer
/// @param iWidth 
/// @param iHeight
void CGBuffer::initialize(int iNumLayer, int iWidth, int iHeight, float fNCP, float fFCP)
{
	debugRecompile();
	m_program.showDebugging();
	if(!setNumberOfLayer(iNumLayer, iWidth, iHeight))
		std::cout << "Some errors on the gbuffer creation" << std::endl;
	m_ProjMatrix90Degrees = glm::perspective(90.f, 1.0f, fNCP, fFCP);
}

void CGBuffer::debugRecompile()
{
	//create the shaders
	m_program.loadShader("shaders/gbuffer.vert", CGLSLProgram::VERTEX);
	m_program.loadShader("shaders/gbuffer.frag", CGLSLProgram::FRAGMENT);
	m_program.create();
	m_program.link();
	m_program.enable();
		m_program.addAttribute("vVertex");
		m_program.addAttribute("vTexCoord");
		m_program.addAttribute("vNormal");
		m_program.addUniform("mModelView");
		m_program.addUniform("mModel");
		//m_program.addUniform("mLookAt");
		m_program.addUniform("cubeMapR0");
		m_program.addUniform("bSpecularOject");
		m_program.addUniform("firstPass");
		m_program.addUniform("debugcolor");
		m_program.addUniform("texSampler");
		m_program.addUniform("iSelection");
		m_program.addUniform("fCenterPosition");
		m_program.addUniform("mView");
		m_program.addUniform("mProjection");
		m_program.addUniform("mNormal");
		m_program.addUniform("bHasTexture");
		//m_program.showDebugging();
	m_program.disable();
}

///
/// Bind the variables to write into the layers
///
void CGBuffer::bindLayer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_uFBOname);	//or GL_DRAW_FRAMEBUFFER
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_uFBODepthName);
	//glPushAttrib(GL_VIEWPORT_BIT | GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, m_iWidth, m_iHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);
	glEnable(GL_DEPTH_TEST);
	GLenum draws [2] ={GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
	glDrawBuffers(2, draws);
}

///
/// Unbind all variables to continue rendering normally
///
/// @param w indicates the width of the normal rendering window
/// @param h indicates the height of the normal rendering window
///
void CGBuffer::unbindLayer(int w, int h)
{
	//glPopAttrib();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
}

///
/// Function which computes 6 view matrix to get the cube projection from a looak position
/// @return a vector of 6 position
///
std::vector<glm::mat4x4> CGBuffer::getViewProjMatrices(const glm::vec3 & vCenter, const glm::vec3 & vAtPosition, const glm::vec3 & vUpCenter)
{
	std::vector<glm::mat4x4> matrices(6);
	glm::vec3 vDistance = vAtPosition - vCenter;
	glm::vec3 vTemp, pZPositive, pYPositive;
	float lenght;

	matrices[0] = glm::lookAt(vCenter, vAtPosition, -vUpCenter);
	pZPositive = vCenter - vDistance;
	matrices[1] = glm::lookAt(vCenter , pZPositive, -vUpCenter);

	//negative X
	lenght = glm::length(vDistance);
	vTemp = glm::cross(glm::normalize(vDistance), vUpCenter) * lenght;
	matrices[2] = glm::lookAt(vCenter , vCenter + vTemp, -glm::cross(glm::normalize(vTemp), glm::normalize(vDistance)));

	matrices[3] = glm::lookAt(vCenter , vCenter - vTemp, -glm::cross(glm::normalize(vTemp), glm::normalize(vDistance)));

	//negative Y
	glm::vec3 value = glm::cross(glm::normalize(vDistance), vUpCenter);
	pYPositive = glm::cross(glm::normalize(vDistance), value);
	matrices[4] = glm::lookAt(vCenter , vCenter + pYPositive,  glm::normalize(vDistance));

	matrices[5] = glm::lookAt(vCenter , vCenter - pYPositive,  -glm::normalize(vDistance));
	std::vector<glm::mat4x4> mSorted(6);	//sorting according to the GL_TEXTURE_CUBE_MAP

	mSorted[0] = matrices[2];
	mSorted[1] = matrices[3];
	mSorted[2] = matrices[5];
	mSorted[3] = matrices[4];
	mSorted[4] = matrices[1];
	mSorted[5] = matrices[0];
	return mSorted;
}


/*
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X 0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y 0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y 0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z 0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 0x851A
*/

///
/// @param vRegularObj An array where normal objects are stored (part of scene)
/// @param vSpecialObj An array where objects which special effect will be applied
/// @param vCenter The center position of camera to lookat
/// @param vAtPosition The at position of camera to lookat
/// @param vUpCenter The up vector of camera to lookat
/// @param mModelMatrix The model matrix of transformation
///
void CGBuffer::createBuffers(std::vector<C3DModel> & vRegularObj, std::vector<C3DModel> & vSpecialObj, const glm::vec3 & vCenter, const glm::vec3 & vAtPosition, const glm::vec3 & vUpCenter, const glm::mat4x4 & mModelMatrix)
{
	GLuint idProgram = m_program.getId();
	glm::mat4x4 mLookAt = glm::lookAt(vCenter, vAtPosition, vUpCenter);
	std::vector<glm::mat4x4> m_vViewMatrices = getViewProjMatrices(vCenter, vAtPosition, vUpCenter);	///< Six matrices to composed the cubemap
	glm::mat3x3 m_normalMatrix;
	std::vector<int>::size_type it = 0;
	
	m_program.enable();
	for (int iLayer = 0; iLayer < m_iNLayers; iLayer++)
	{
		//start from texture 0, 1 for layer 0; texture 1, 2 for layer 1, and so on
		int tex1 = iLayer << 1;
		int tex2 = tex1 + 1;
		for (int k = 0; k < 6; k++)
		{
			//1st texture of layer
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, getCubeMapId(tex1));
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + k, getCubeMapId(tex1), 0);
			//2nd texture of layer
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_CUBE_MAP, getCubeMapId(tex2));
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_CUBE_MAP_POSITIVE_X + k, getCubeMapId(tex2), 0);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(0,0,0,1);

			if(iLayer > 0)	//start from the 2nd layer
			{
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_CUBE_MAP, getCubeMapId(tex1 - 1));
				//std::cout << "back layer " << (l1 - 1) << std::endl;
				glProgramUniform1i(idProgram, m_program.getLocation("cubeMapR0"), 2);
				//std::cout << "layer:" << ((iLayer-1)<<1)-1 << std::endl;
			}
			//printOpenGLError2();
			glProgramUniform1i(idProgram, m_program.getLocation("firstPass"), (iLayer == 0?true:false));
			glProgramUniform1i(idProgram, m_program.getLocation("bSpecularOject"), 0);
			glProgramUniform1i(idProgram, m_program.getLocation("texSampler"), 7);
			glUniform3fv(m_program.getLocation("fCenterPosition"),1, glm::value_ptr(vCenter));
			glUniformMatrix4fv(m_program.getLocation("mView"), 1, GL_FALSE, glm::value_ptr(m_vViewMatrices[k]));
			//glUniformMatrix4fv(m_program.getLocation("mLookAt"), 1, GL_FALSE, glm::value_ptr(mLookAt));
			//glUniformMatrix4fv(m_program.getLocation("mModelView"), 1, GL_FALSE, glm::value_ptr(mModelViewMatrix));
			glUniformMatrix4fv(m_program.getLocation("mModel"), 1, GL_FALSE, glm::value_ptr(mModelMatrix));
			glUniformMatrix4fv(m_program.getLocation("mProjection"), 1, GL_FALSE, glm::value_ptr(m_ProjMatrix90Degrees));
			glm::mat4x4 mModelViewMatrix = m_vViewMatrices[k] * mModelMatrix;
			m_normalMatrix = glm::mat3x3(glm::transpose(glm::inverse(mModelViewMatrix)));
			glUniformMatrix3fv(m_program.getLocation("mNormal"), 1, GL_FALSE, glm::value_ptr(m_normalMatrix));
			
			//printOpenGLError2();
			glEnableVertexAttribArray(m_program.getLocation("vVertex"));
			//printOpenGLError2();
			if(m_program.getLocation("vNormal") != -1) glEnableVertexAttribArray(m_program.getLocation("vNormal"));
			//printOpenGLError2();
			if(m_program.getLocation("vTexCoord") != -1)	glEnableVertexAttribArray(m_program.getLocation("vTexCoord"));
			//printOpenGLError2();
			if(m_program.getLocation("vColor") != -1)	glEnableVertexAttribArray(m_program.getLocation("vColor"));
			//printOpenGLError2();
			glActiveTexture(GL_TEXTURE7);
			glUniform1i(m_program.getLocation("sTexture"), 7);
			glUniform1i(m_program.getLocation("bHasTexture"), 1);
			for(it = 0; it != vRegularObj.size(); it++)
				vRegularObj[it].draw();
			glUniform1i(m_program.getLocation("bSpecularOject"), 1);
				
//			for(it = 0; it != vSpecialObj.size(); it++) 
//				vSpecialObj[it].draw();
			//printOpenGLError2();
			glDisableVertexAttribArray(m_program.getLocation("vVertex"));
			if(m_program.getLocation("vNormal") != -1)	glDisableVertexAttribArray(m_program.getLocation("vNormal"));
			if(m_program.getLocation("vTexCoord") != -1)	glDisableVertexAttribArray(m_program.getLocation("vTexCoord"));
			if(m_program.getLocation("vColor") != -1)	glDisableVertexAttribArray(m_program.getLocation("vColor"));

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			glActiveTexture(GL_TEXTURE7);
			glBindTexture(GL_TEXTURE_2D, 0);
			//printOpenGLError2();
		}
	}
	m_program.disable();
}


void CGBuffer::debugCubeMap(unsigned int idCubeMap)
{
#ifdef _WIN32	
	float *p = new float[512 * 512 * 4];
	glBindTexture(GL_TEXTURE_CUBE_MAP, idCubeMap);
	glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, GL_FLOAT, p);
	imdebug("rgba b=32f w=512 h=512 %p", p);
	glGetTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, GL_FLOAT, p);
	imdebug("rgba b=32f w=512 h=512 %p", p);
	glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, GL_FLOAT, p);
	imdebug("rgba b=32f w=512 h=512 %p", p);
	glGetTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, GL_FLOAT, p);
	imdebug("rgba b=32f w=512 h=512 %p", p);
	glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, GL_FLOAT, p);
	imdebug("rgba b=32f w=512 h=512 %p", p);
	glGetTexImage(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, GL_FLOAT, p);
	imdebug("rgba b=32f w=512 h=512 %p", p);
  delete [] p;
 #endif
}
