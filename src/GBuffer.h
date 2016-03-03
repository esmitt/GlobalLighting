#include "GLSLProgram.h"
#include "glm/glm.hpp"
#include "loader/3DModel.h"
#include <vector>

class CApplication;
typedef void (CApplication::*renderFnc)();

#define MAX_NUMBER_OF_TEXTURES 8	//the number of available number of textures to create

class CGBuffer
{
private:
	int m_iNLayers;
	int m_iNTextures;
	int m_iWidth, m_iHeight;			///indicates the size of the textures (power of 2?)
	unsigned int* m_puCubeTexId;	///indexes to the textures
	unsigned int m_uFBOname, m_uFBODepthName;	/// values of the framebuffers (values & depth)
	CGLSLProgram m_program;				///program to store the values in the rendering
	glm::mat4x4 m_ProjMatrix90Degrees;
private:
	bool setNumberOfLayer(int k, int width, int height);
	std::vector<glm::mat4x4> getViewProjMatrices(const glm::vec3 & vCenter, const glm::vec3 & vAtPosition, const glm::vec3 & vUpCenter);
	void debugCubeMap(unsigned int idCubeMap);
	
public:
	CGBuffer(void);
	~CGBuffer(void);
	void initialize(int iNumLayer, int iWidth, int iHeight, float fNCP, float fFCP);
	void bindLayer();
	//void makeLayer(int layer, functiontype renderFunction);
	void unbindLayer(int w, int h);
	inline unsigned int getCubeMapId(int l){return m_puCubeTexId[l];}
	inline int getNumberOfLayer(){return m_iNLayers;}
	CGLSLProgram & getGLSLProgram(){return m_program;}
	void createBuffers(std::vector<C3DModel> & vRegularObj, std::vector<C3DModel> & vSpecialObj, const glm::vec3 & vCenter, const glm::vec3 & vAtPosition, const glm::vec3 & vUpCenter, const glm::mat4x4 & mModelMatrix);
	void debugRecompile();
};
