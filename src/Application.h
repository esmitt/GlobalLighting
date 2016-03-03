#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "GBuffer.h"
#include "loader/3DModel.h"
#include "Camera.h"
#include "FullScreenQuad.h"


using namespace std;
#ifdef _WIN32
	#pragma comment(lib, "lib/glfw3dll.lib")
	#pragma comment(lib, "lib/glew32.lib")
	#pragma comment(lib, "opengl32.lib")
#endif

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

class CApplication
{
public:
	//static const int HAS_TEXTURE = 1;
	CApplication();
	~CApplication();
	void setDimensions(int iWidth, int iHeight);
	bool initialize();
	void update(double time);
	void destroy();
	int closeApp();
	void run();
	static CApplication& getInstance(); // Singleton is accessed via getInstance()
private:
	unsigned int m_iWidth;
	unsigned int m_iHeight;
	unsigned int m_iNLayer;
	float m_fNCP, m_fFCP, m_fAngle;
	string m_strNameWindow;
	GLFWwindow* m_glfwWindow;
	CGBuffer m_gbuffer;
	CCamera m_camera;
	CFullScreenQuad m_fullQuad;
	
	//control variables
	bool m_bTakeCubemaps;
	bool m_bDebugging;

	//local variables
	CGLSLProgram m_progBase;
	CGLSLProgram m_progRayTrace;
	glm::mat4x4 mProjMatrix, mModelViewMatrix, mModelMatrix;
	//C3DModel* m_Model;
	std::vector<C3DModel> m_vRegularObj;
	std::vector<C3DModel> m_vSpecialObj;
	float m_colorBckg[4];

	//functions
	CApplication(CApplication const&); // prevent copies
  void operator=(CApplication const&); // prevent assignments
	static void keyboardCallback(GLFWwindow* window, int iKey, int iScancode, int iAction, int iMods);
	static void resizeCallback(GLFWwindow* window, int iWidth, int iHeight);
	void firstPhase();
	void secondPhase();
private:
	void draw();
	void recompileRayTracer();
private:
	//static void resizeCB(GLFWwindow* window, int iWidth, int iHeight);
	//static void keyboardCB(GLFWwindow* window, int iKey, int iScancode, int iAction, int iMods);
	//static void errorCB(int error, const char* description);
	void resizeCB(GLFWwindow* window, int iWidth, int iHeight);
	void keyboardCB(GLFWwindow* window, int iKey, int iScancode, int iAction, int iMods);
	static void errorCB(int error, const char* description);
	void debugCubeMap(unsigned int idCubeMap);
	void normalRendering();
	void effectRendering();
};

#endif
