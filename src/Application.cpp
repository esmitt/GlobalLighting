#include "Application.h"
#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"
#ifdef _WIN32
	#include <imdebug/imdebug.h>
	#include <imdebug/imdebuggl.h>
	#pragma comment(lib, "imdebug.lib")
#endif

CApplication::CApplication()
{
	m_strNameWindow = "Hello GLFW";//the name
	m_fNCP = 0.01f;
	m_fFCP = 500.0f;
	m_iWidth = 512;
	m_iHeight = 512;
	m_fAngle = 45.f;
	m_iNLayer = 1;
	m_bDebugging = m_bTakeCubemaps = false;
}

CApplication::~CApplication()
{
	m_vRegularObj.clear();
	m_vSpecialObj.clear();
}

CApplication& CApplication::getInstance()
{
	static CApplication instance; // lazy singleton, instantiated on first use
  return instance;
}

void CApplication::setDimensions(int iWidth, int iHeight)
{
	m_iWidth = iWidth;
	m_iHeight = iHeight;
}

void CApplication::keyboardCallback(GLFWwindow* window, int iKey, int iScancode, int iAction, int iMods)
{
	getInstance().keyboardCB(window, iKey, iScancode, iAction, iMods); 
}

///
/// The keyboard function call back
/// @param window id of the window that received the event
/// @param iKey the key pressed or released
/// @param iScancode the system-specific scancode of the key.
/// @param iAction can be GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT
/// @param iMods Bit field describing which modifier keys were held down (Shift, Alt, & so on)
///
void CApplication::keyboardCB(GLFWwindow* window, int iKey, int iScancode, int iAction, int iMods)
{
	if (iAction == GLFW_PRESS || iAction == GLFW_REPEAT)
	{
		switch (iKey)
		{
		case GLFW_KEY_ESCAPE:
		case GLFW_KEY_Q:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_A:
			m_camera.moveCamera2(-m_camera.getMouseSpeed());
			m_camera.setViewByMouse();
			break;
		case GLFW_KEY_S:
			m_camera.moveCamera(-m_camera.getMouseSpeed());
			m_camera.setViewByMouse();
			break;
		case GLFW_KEY_D:
			m_camera.moveCamera2(m_camera.getMouseSpeed());
			m_camera.setViewByMouse();
			break;
		case GLFW_KEY_W:
			m_camera.moveCamera(m_camera.getMouseSpeed());
			m_camera.setViewByMouse();
			break;
		case GLFW_KEY_P:
			m_camera.captureMouse();
			break;
		case GLFW_KEY_F1:
			std::cout << "pos: " << m_camera.getCenter().x << ", " << m_camera.getCenter().y << ", " << m_camera.getCenter().z << std::endl;
			std::cout << "at: " << m_camera.getAtPosition().x << ", " << m_camera.getAtPosition().y << ", " << m_camera.getAtPosition().z << std::endl;
			std::cout << "up: " << m_camera.getUpVector().x << ", " << m_camera.getUpVector().y << ", " << m_camera.getUpVector().z << std::endl;
			break;
		case GLFW_KEY_2:
			m_bTakeCubemaps = !m_bTakeCubemaps;
			break;
		case GLFW_KEY_SPACE:
			m_bDebugging = !m_bDebugging;
			break;
		case GLFW_KEY_F2:
			m_gbuffer.debugRecompile();
			break;
		case GLFW_KEY_F3:
			m_progRayTrace.loadShader("shaders/raytracer.vert", CGLSLProgram::VERTEX);
			m_progRayTrace.loadShader("shaders/raytracer.frag", CGLSLProgram::FRAGMENT);
			m_progRayTrace.create_link();
			m_progRayTrace.enable();
				m_progRayTrace.addAttribute("vVertex");
				m_progRayTrace.addAttribute("vTexCoord");
				m_progRayTrace.addUniform("sSamplerCube0");
				m_progRayTrace.addUniform("sSamplerCube1");
				m_progRayTrace.addUniform("sSamplerCube2");
				m_progRayTrace.addUniform("sSamplerCube3");
				m_progRayTrace.addUniform("sSamplerCube4");
				m_progRayTrace.addUniform("sSamplerCube5");
		m_progRayTrace.disable();
		break;
		case GLFW_KEY_0:
			break;
		}
	}
}

void CApplication::recompileRayTracer()
{

}

void CApplication::resizeCallback(GLFWwindow* window, int iWidth, int iHeight)
{
	getInstance().resizeCB(window, iWidth, iHeight);
}

///< The resizing function
void CApplication::resizeCB(GLFWwindow* window, int iWidth, int iHeight)
{
	if(iHeight == 0) iHeight = 1;
	float ratio = iWidth / float(iHeight);
	glViewport(0, 0, iWidth, iHeight);
	mProjMatrix = glm::perspective(m_fAngle, ratio, m_fNCP, m_fFCP);
	m_iWidth = iWidth;
	m_iHeight = iHeight;
}

///
/// Init all data and variables.
/// @return true if everything is ok, false otherwise
///
bool CApplication::initialize()
{
	if (!glfwInit())	return false;
	glfwSetErrorCallback(errorCB);
	//glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
	//glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
	//glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
	m_glfwWindow = glfwCreateWindow(m_iWidth, m_iHeight, m_strNameWindow.c_str(), NULL, NULL);
	if (!m_glfwWindow) return false;
	//glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(m_glfwWindow, m_iWidth/2.0, m_iHeight/2.0);
	//glfwDisable(MOUSE_CURSOR);
	glfwMakeContextCurrent(m_glfwWindow);
	
	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK) return false;
	// Setup ImGui binding
	ImGui_ImplGlfwGL3_Init(m_glfwWindow, true);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "==========================================" << std::endl;

	//resizeCB(m_glfwWindow, m_iWidth, m_iHeight);	//just the 1st time
	resizeCallback(m_glfwWindow,  m_iWidth, m_iHeight);	//just the 1st time
	glfwSetKeyCallback(m_glfwWindow, &CApplication::keyboardCallback);
	glfwSetWindowSizeCallback(m_glfwWindow, &CApplication::resizeCallback);
	
	//load model
	m_vRegularObj.reserve(1);
	C3DModel* model = new C3DModel();
	//if(m_Model->load("Battlelord_DNF/Battlelord.obj", TypeObject::SPECULAR, glm::vec3(0,0,-2)))
	//if(m_Model->load("Battlelord_DNF/Battlelord.obj", TypeObject::SPECULAR))
	//if(model->load("Mario and Luigi/luigi.obj", TypeObject::SPECULAR))
	//if(model->load("objects/homers/Homer Santa/Homer_Santa.obj", TypeObject::SPECULAR))
	//if(m_Model->load("capsule.obj", TypeObject::SPECULAR))
	if(model->load("assets/sponza.obj", TypeObject::SPECULAR))
	//if(model->load("objects/dragon.obj", TypeObject::SPECULAR))
	{
		cout << "model load successfully" << endl;
		cout << "center: " << model->getCenter().x << " " << model->getCenter().y << " " << model->getCenter().z << endl;
	}
	else
	{
		cout << "model failed" << endl;
		return false;
	}
	m_vRegularObj.push_back(*model);
	//load the shaders
	m_progBase.loadShader("shaders/basic.vert", CGLSLProgram::VERTEX);
  m_progBase.loadShader("shaders/basic.frag", CGLSLProgram::FRAGMENT);
  m_progBase.create_link();
	m_progBase.enable();
		m_progBase.addAttribute("vVertex");
		m_progBase.addAttribute("vNormal");
		m_progBase.addAttribute("vColor");
		m_progBase.addAttribute("vTexCoord");
		m_progBase.addUniform("mProjection");
		m_progBase.addUniform("sTexture");
		m_progBase.addUniform("bHasTexture");
		m_progBase.addUniform("mModelView");
  m_progBase.disable();

	m_progRayTrace.loadShader("shaders/raytracer.vert", CGLSLProgram::VERTEX);
  m_progRayTrace.loadShader("shaders/raytracer.frag", CGLSLProgram::FRAGMENT);
	m_progRayTrace.create_link();
	m_progRayTrace.enable();
		m_progRayTrace.addAttribute("vVertex");
		m_progRayTrace.addAttribute("vTexCoord");
		m_progRayTrace.addUniform("sSamplerCube0");
		m_progRayTrace.addUniform("sSamplerCube1");
		m_progRayTrace.addUniform("sSamplerCube2");
		m_progRayTrace.addUniform("sSamplerCube3");
		m_progRayTrace.addUniform("sSamplerCube4");
		m_progRayTrace.addUniform("sSamplerCube5");
	m_progRayTrace.disable();

	//set the matrices to the identity
	mModelMatrix = glm::mat4x4(1);
	mModelViewMatrix = glm::mat4x4(1);
	//initialize the gbuffer
	m_gbuffer.initialize(m_iNLayer, m_iWidth, m_iHeight, m_fNCP, m_fFCP);
	//m_gbuffer.initialize(m_iNLayer, 1024, 1024, m_fNCP, m_fFCP);
	//initialize the camer
	m_camera.initCamera(m_iWidth, m_iHeight, m_glfwWindow);
	//m_camera.setMouseSpeed(model->getDiagonal() /10000.f);
	m_camera.setMouseSpeed(model->getDiagonal() /1000.f);
	//m_camera.setPositionCamera(glm::vec3( -0.166939, -0.100085, 0.0201825), glm::vec3(-0.892258, -0.271238, 0.68699), glm::vec3(0,1,0));
	//delete
	model = NULL;
	delete model;

	m_fullQuad.initialize();

	return true;
}

/// Here all data must be destroyed + glfwTerminate
void CApplication::destroy()
{
	//delete m_Model;
	glfwDestroyWindow(m_glfwWindow);
	ImGui_ImplGlfwGL3_Shutdown();
	glfwTerminate();
}

int CApplication::closeApp()
{
	return glfwWindowShouldClose(m_glfwWindow);
}

void CApplication::normalRendering()
{
	/*for(std::vector<C3DModel>::iterator it = m_vRegularObj.begin(); it != m_vRegularObj.end(); ++it)
		it->draw();*/
	for(std::vector<int>::size_type it = 0; it != m_vRegularObj.size(); it++)
	{
		//glUniform1i(m_progBase.getLocation("bHasTexture"),  m_vRegularObj[it].hasTexture());	//0 false, otherwise true
		glUniform1i(m_progBase.getLocation("bHasTexture"),  1);	//0 false, otherwise true
		m_vRegularObj[it].draw();
	}
}

void CApplication::effectRendering()
{
	for(std::vector<C3DModel>::iterator it = m_vSpecialObj.begin(); it != m_vSpecialObj.end(); ++it)
		it->draw();
}
	
//glm::vec3 center(0); //center
//glm::vec3 at = glm::vec3(0,1,-2); //at
//glm::vec3 up = glm::vec3(0,1,0); //up
glm::vec3 center(0.0762785f, -0.0849742f, 0); //center
glm::vec3 at = glm::vec3(1.f, -0.12796, 0); //at
glm::vec3 up = glm::normalize(glm::cross(at, center));

void CApplication::firstPhase()
{

}

void CApplication::secondPhase()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_gbuffer.getCubeMapId(0));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_gbuffer.getCubeMapId(1));
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_gbuffer.getCubeMapId(2));
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_gbuffer.getCubeMapId(3));
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_gbuffer.getCubeMapId(4));
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_gbuffer.getCubeMapId(5));
	m_progRayTrace.enable();
		glUniform1i(m_progRayTrace.getLocation("sSamplerCube0"), 0);
		glUniform1i(m_progRayTrace.getLocation("sSamplerCube1"), 1);
		glUniform1i(m_progRayTrace.getLocation("sSamplerCube2"), 2);
		glUniform1i(m_progRayTrace.getLocation("sSamplerCube3"), 3);
		glUniform1i(m_progRayTrace.getLocation("sSamplerCube4"), 4);
		glUniform1i(m_progRayTrace.getLocation("sSamplerCube5"), 5);
		m_fullQuad.draw();
	m_progRayTrace.disable();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

/// Draw function in 2 stages: <br>
/// 1) Create the gbufferv
/// 2) Using data to display the scene using FSQAA
void CApplication::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.15f, 0.15f, 0.15f, 1.f);

	mModelViewMatrix = glm::translate(glm::mat4(), -m_vRegularObj[0].getCenter());
	float d = 1.f/m_vRegularObj[0].getDiagonal();
	mModelViewMatrix =  glm::scale(glm::mat4(), glm::vec3(d,d,d)) * mModelViewMatrix;
	mModelMatrix = mModelViewMatrix;
	//mModelViewMatrix = glm::lookAt(center, at,up) * mModelViewMatrix;
	//mModelViewMatrix = glm::translate(glm::mat4(), -m_Model->getCenter() - glm::vec3(0,0,5)); 
	glm::mat4 lookat = m_camera.getLookAt();
	//////double o = glfwGetTime()*50 ;
	//////mModelViewMatrix  = glm::rotate(glm::mat4(), float(o) , glm::vec3(0,1,0)) * mModelViewMatrix; 
	//////mModelViewMatrix = glm::translate(glm::mat4(), glm::vec3(0,0,-.100)) * mModelViewMatrix;
	
	mModelViewMatrix = lookat * mModelViewMatrix;
	
	//mModelViewMatrix = glm::translate(glm::mat4(), glm::vec3(0,0,-5.f)); 
	//m_Model->centerOnPoint(glm::vec3(0,0,-2));
	glActiveTexture(GL_TEXTURE7);
	m_progBase.enable();
		glUniformMatrix4fv(m_progBase.getLocation("mModelView"), 1, GL_FALSE, glm::value_ptr(mModelViewMatrix));
		glUniformMatrix4fv(m_progBase.getLocation("mProjection"), 1, GL_FALSE, glm::value_ptr(mProjMatrix));	
		glUniform1i(m_progBase.getLocation("sTexture2D"), 7);
		glEnableVertexAttribArray(m_progBase.getLocation("vVertex"));
		if(m_progBase.getLocation("vNormal") != -1) glEnableVertexAttribArray(m_progBase.getLocation("vNormal"));
		if(m_progBase.getLocation("vTexCoord") != -1) glEnableVertexAttribArray(m_progBase.getLocation("vTexCoord"));
		if(m_progBase.getLocation("vColor") != -1) glEnableVertexAttribArray(m_progBase.getLocation("vColor"));
		glActiveTexture(GL_TEXTURE7);
		glUniform1i(m_progBase.getLocation("sTexture"), 7);
		normalRendering();
		glDisableVertexAttribArray(m_progBase.getLocation("vVertex"));
		if(m_progBase.getLocation("vNormal") != -1) glDisableVertexAttribArray(m_progBase.getLocation("vNormal"));
		if(m_progBase.getLocation("vTexCoord") != -1) glDisableVertexAttribArray(m_progBase.getLocation("vTexCoord"));
		if(m_progBase.getLocation("vColor") != -1) glDisableVertexAttribArray(m_progBase.getLocation("vColor"));
  m_progBase.disable();
  /*
  m_progBase.enable();
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glEnableVertexAttribArray(3);
			glUniformMatrix4fv(m_progBase.getLocation("mModelView"), 1, GL_FALSE, glm::value_ptr(mModelViewMatrix));
			glUniformMatrix4fv(m_progBase.getLocation("mProjection"), 1, GL_FALSE, glm::value_ptr(mProjMatrix));
			glActiveTexture(GL_TEXTURE1);
			glUniform1i(m_progBase.getLocation("sTexture"), 1);
			glUniform1i(m_progBase.getLocation("bHasTexture"), model->hasTexture());	//0 false, otherwise true
			//std::cout << model->hasTexture()<<std::endl;
			model->draw();
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
			glDisableVertexAttribArray(3);
    m_progBase.disable();
  */

	if(m_bTakeCubemaps)
	{
		//testing the gbuffer
		m_gbuffer.bindLayer();
		//m_gbuffer.createBuffers(normalRendering, this->effectRendering, mModelViewMatrix, glm::vec3(0), glm::vec3(0,1,-2), glm::vec3(0,1,0));

		//m_gbuffer.createBuffers(m_vRegularObj, m_vSpecialObj, mModelViewMatrix, a, b, c);
		m_gbuffer.createBuffers(m_vRegularObj, m_vSpecialObj, m_camera.getCenter(), m_camera.getAtPosition(), m_camera.getUpVector(), mModelMatrix);
			//m_progBase.enable();
			//	glUniformMatrix4fv(m_progBase.getLocation("mModelView"), 1, GL_FALSE, glm::value_ptr(mModelViewMatrix));
			//	glUniformMatrix4fv(m_progBase.getLocation("mProjection"), 1, GL_FALSE, glm::value_ptr(mProjMatrix));	
			//	glUniform1i(m_progBase.getLocation("sTexture2D"), 7);
			//	glEnableVertexAttribArray(m_progBase.getLocation("vVertex"));
			//	if(m_progBase.getLocation("vNormal") != -1) glEnableVertexAttribArray(m_progBase.getLocation("vNormal"));
			//	if(m_progBase.getLocation("vTexCoord") != -1) glEnableVertexAttribArray(m_progBase.getLocation("vTexCoord"));
			//	
			//	//m_Model->draw();
			//	//m_vRegularObj[0].draw();
			//	glDisableVertexAttribArray(m_progBase.getLocation("vVertex"));
			//	if(m_progBase.getLocation("vNormal") != -1) glDisableVertexAttribArray(m_progBase.getLocation("vNormal"));
			//	if(m_progBase.getLocation("vTexCoord") != -1) glDisableVertexAttribArray(m_progBase.getLocation("vTexCoord"));
			//m_progBase.disable();
		m_gbuffer.unbindLayer(m_iWidth, m_iHeight);
		m_bTakeCubemaps = false;
		debugCubeMap(m_gbuffer.getCubeMapId(0));
		//debugCubeMap(m_gbuffer.getCubeMapId(2));
		//debugCubeMap(m_gbuffer.getCubeMapId(4));
	}

	//float *p = new float[512 * 512 * 4];
	//glBindTexture(GL_TEXTURE_CUBE_MAP, idCubeMap);
	//glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, GL_FLOAT, p);
	//imdebug("rgba b=32f w=512 h=512 %p", p);
	
	//second phase
	//secondPhase();
	//debugCubeMap(0);

	
}

/// Function to be executed on each iteration
void CApplication::run()
{
	draw();
	ImGui_ImplGlfwGL3_NewFrame();

	// 1. Show a simple window
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
	{
		bool show_test_window = true;
		bool show_another_window = false;
		ImVec4 clear_color = ImColor(114, 144, 154);
		static float f = 0.0f;
		ImGui::Text("Hello, world!");
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		ImGui::ColorEdit3("clear color", (float*)&clear_color);
		if (ImGui::Button("Test Window")) show_test_window ^= 1;
		if (ImGui::Button("Another Window")) show_another_window ^= 1;
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::Render();
	}
	
	glfwSwapBuffers(m_glfwWindow);
	glfwPollEvents();	//or glfwWaitEvents()
}

void CApplication::errorCB(int error, const char* description)
{
	cout << description << endl;
}

void CApplication::update(double time)
{
	if(m_camera.isActivate())
	{
		const float mouseSensitivity = 0.1f;
		double mouseX, mouseY;
		int iMouseX, iMouseY;
		glfwGetCursorPos(m_glfwWindow, &mouseX, &mouseY);
		//std::cout << mouseX << "," << mouseY << std::endl;
		iMouseX = (int)(mouseX + 0.5);
		iMouseY = (int)(mouseY + 0.5);
		m_camera.setMousePosition(iMouseX, iMouseY);
		m_camera.setViewByMouse();
		//glfwSetCursorPos(m_glfwWindow, 0.0, 0.0);
	}
}

void CApplication::debugCubeMap(unsigned int idCubeMap)
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
