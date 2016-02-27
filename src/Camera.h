#pragma once
#include "glm/glm.hpp" // glm::vec3, glm::vec4, glm::ivec4, glm::mat4
#include "glm/gtc/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "glm/gtc/type_ptr.hpp" // glm::value_ptr
#include "glm/glm.hpp"
#ifdef _WIN32
    #include "GLFW/glfw3.h" 
    #pragma comment(lib, "lib/glfw3dll.lib")
#else 
    #include <GLFW/glfw3.h>
#endif

using namespace std;

class CCamera
{
private:
	float m_fCurrentRotX;
	glm::vec3 m_pEye;
	glm::vec3 m_pAt;
	glm::vec3 m_vUp;
	glm::ivec2 m_pMousePos;
	int m_iWidth, m_iHeight;
	float m_fTolerance;
	bool m_bCapture;
	glm::mat4 m_Identity;
	GLFWwindow* m_glfwWindow;
	float MOUSE_SPEED;
public:

	CCamera(void);
	CCamera(int iWidth, int iHeight, GLFWwindow* glfwWindow);
	~CCamera(void);
	
	///Set the Camera the width/height of the screen.
	void initCamera(int iWidth,int iHeight, GLFWwindow* glfwWindow);

	///Set the width/height of the screen.
	void setDimensions(int iWidth,int iHeight);

	///Change the position of the mouse on the screen.
	void setMousePosition(int iX, int iY);

	///Set the lookat property in a render function.
	//void setLookAt();

	///Set the lookat property in a render function.
	void setPositionCamera(glm::vec3 pPosition, glm::vec3 vView, glm::vec3 vUp);

	///Calculate the rotation of the Camera cause by the movement of the mouse.
	void setViewByMouse();

	///Set the rotation of the Camera cause by the movement of the mouse.
	void rotateView(float fAngle, float fX, float fY, float fZ);

	///Move the Camera with a given speed.
	void moveCamera(float fSpeed);

	///Move the Camera with a given speed.
	void moveCamera2(float fSpeed);

	///Move vertical with a given speed.
	void moveCameraVertical(float fSpeed);

	///Active or desactive the capture of the mouse.
	void captureMouse();

	///Get the look at matriz from the Camera
	inline glm::mat4 getLookAt(){return glm::lookAt(m_pEye, m_pAt, m_vUp);}

	///Get the position of the Camera in world Coordinates
	inline glm::vec3 getCenter(){return m_pEye;}

	//set the position
	inline void setCenter(glm::vec3 pCenter){m_pEye = pCenter;}

	//set the view vector
	inline void setAtVector(glm::vec3 pAt){m_pAt = pAt;}
	
	//set the up vector
	inline void setUpVector(glm::vec3 vUp){m_vUp = vUp;}

	//return the status of the mouse's capture
	inline bool isActivate(){return m_bCapture;}

	//get the width stored
	inline int getWidth(){return m_iWidth;}

	//get the height stored
	inline int getHeight(){return m_iHeight;}

	//get at vector
	inline glm::vec3 getAtPosition(){return m_pAt;}

	//get up vector
	inline glm::vec3 getUpVector(){return m_vUp;}

	///set the mouse step
	inline void setMouseSpeed(float fStep){MOUSE_SPEED = fStep;}

	///get the speed of the mouse
	inline float getMouseSpeed(){return MOUSE_SPEED;}
};

