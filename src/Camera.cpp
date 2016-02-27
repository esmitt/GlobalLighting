#include "Camera.h"

using namespace std;

//#include "GL\freeglut.h"
//#pragma comment(lib, "freeglut.lib")

///
/// Default method to construct a Camera.
///
///
CCamera::CCamera(void) : m_bCapture(false), m_Identity (glm::mat4x4(1.0))
{
	//ShowCursor(false);
	m_bCapture = false;
	//ShowCursor(true);
	MOUSE_SPEED = 0.01f;
}

///
/// Default destructor
///
CCamera::~CCamera(void)
{
}

///
/// Method to construct a Camera with the width/height of the screen.
///
/// @params iWidth widht of the screen.
/// @params iHeight height of the sceen.
///
CCamera::CCamera(int iWidth, int iHeight, GLFWwindow* glfwWindow)
{
	initCamera(iWidth, iHeight, glfwWindow);
	m_bCapture = false;
	m_Identity = glm::mat4x4(1.0);
}

///
/// Method to set the Camera the width/height of the screen.
///
/// @params iWidth widht of the screen.
/// @params iHeight height of the sceen.
///
void CCamera::initCamera(int iWidth,int iHeight, GLFWwindow* glfwWindow)
{
	//Inicializador de la clase
	m_iWidth = iWidth;
	m_iHeight = iHeight;

	m_fTolerance = 1.5f;

	/*m_pPosition = glm::vec3(0.0f, 0.0f, 0.0f);	
	m_vView = glm::vec3(0.0, -1.0, 0.0);	
	m_vUp = glm::vec3(0.0, 1.0, 0.0);	*/
	
	m_pEye = glm::vec3(0.0f, 0.0f, 0.0f);	
	m_pAt = glm::vec3(0.0, 0.0, -1.0);	
	m_vUp = glm::vec3(0.0, 1.0, 0.0);	

	m_fCurrentRotX = 0.0;
	m_glfwWindow = glfwWindow;

	//Hide the mouse
	//ShowCursor(false);
	glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	m_bCapture = false;
	glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	//ShowCursor(true);
}

///
/// Method to set the width ande height of the screen.
///
/// @params iWidth widht of the screen.
/// @params iHeight height of the sceen.
///
void CCamera::setDimensions(int iWidth,int iHeight)
{
	m_iWidth = iWidth;
	m_iHeight = iHeight;
}

///
/// Method to change the position of the mouse on the screen.
///
/// @params iX X position of the screen.
/// @params iY Y position of the sceen.
///
void CCamera::setMousePosition(int iX, int iY)
{
	m_pMousePos.x = iX;
	m_pMousePos.y = iY;
}

///
/// Method to set the lookat property in a render function.
///
//void CCamera::setLookAt()
//{
//	glLoadMatrixf(glm::value_ptr(glm::lookAt(m_pPosition,m_pView,m_pUpVector)));
//}

/**
* Method to set the lookat property in a render function.
*
* @params pPosition X/Y/Z position of the Camera
* @params vView X/Y/Z component of the view vector
* @params vUp X/Y/Z component of the up vector
*/
void CCamera::setPositionCamera(glm::vec3 pCenter, glm::vec3 pAt, glm::vec3 vUp)
{
	m_pEye = pCenter;
	m_pAt = pAt;
	m_vUp = vUp;
}

///
/// Method to calculate the rotation of the Camera cause by the movement of the mouse.
///
///
void CCamera::setViewByMouse()
{
	if(m_bCapture){
		int middleX = m_iWidth >> 1;	
		int middleY = m_iHeight >> 1;	
		float angleY = 0.0f; //Rotation axis left-right
		float angleZ = 0.0f; //Rotation axis top-down
		
		//If the mouse hasn't move, don't do anything
		if( (m_pMousePos.x == middleX) && (m_pMousePos.y == middleY) ) return;

		//Put the mouse in the center of the screen
		//glutWarpPointer(middleX,middleY);	
		//glfwSetCursorPos(m_glfwWindow, 0.0, 0.0);
		glfwSetCursorPos(m_glfwWindow,middleX,middleY);

		//Calculate the rotation angle with a tolerance
		angleY = (float)( (middleX - m_pMousePos.x) ) * 0.001f;
		angleZ = (float)( (middleY - m_pMousePos.y) ) * 0.001f;
		
		float lastRotX = 0.0f; 
		lastRotX = m_fCurrentRotX;

		m_fCurrentRotX += angleZ;
	 
		//Calculate the axis for the rotation
		glm::vec3 vAxis = m_pAt - m_pEye;
		vAxis = glm::cross(vAxis, m_vUp);
		vAxis = glm::normalize(vAxis);

		
		if(m_fCurrentRotX > 1.0f){
			//There is a maximun in the top
			m_fCurrentRotX = 1.0f;
			if(lastRotX != 1.0f)		
				rotateView( 1.0f - lastRotX, vAxis.x, vAxis.y, vAxis.z);

		}else if(m_fCurrentRotX <= -1.0f){
			//There is a minimun down
			m_fCurrentRotX = -1.0f;
			if(lastRotX != -1.0f)
				rotateView( -1.0f - lastRotX, vAxis.x, vAxis.y, vAxis.z);

		}else{
			//Rotate freely
			rotateView(angleZ, vAxis.x, vAxis.y, vAxis.z);
		}

		//Rotate in the Y angle
		rotateView(angleY, 0, 1, 0);
	}
}

///
/// Method to set the rotation of the Camera cause by the movement of the mouse.
///
/// @params fAngle amount of rotation
/// @params fX X component of the rotation vector
/// @params fY Y component of the rotation vector
/// @params fZ Z component of the rotation vector
///
void CCamera::rotateView(float fAngle, float fX, float fY, float fZ)
{
	if(m_bCapture){
		glm::vec3 vNewView;

		glm::vec3 vView = m_pAt - m_pEye;

		//Calculate sin and cosine of the angle
		float cosTheta = (float)cos(fAngle);
		float sinTheta = (float)sin(fAngle);

		//Calculate the new X of the rotation
		vNewView.x = (	((cosTheta + (1 - cosTheta) * fX * fX)		* vView.x) +
							(((1 - cosTheta) * fX * fY - fZ * sinTheta)	* vView.y) +
							(((1 - cosTheta) * fX * fZ + fY * sinTheta)	* vView.z));

		//Calculate the new Y of the rotation
		vNewView.y = (	(((1 - cosTheta) * fX * fY + fZ * sinTheta)	* vView.x) +
							(((cosTheta + (1 - cosTheta) * fY * fY)		* vView.y) +
							(((1 - cosTheta) * fY * fZ - fX * sinTheta)	* vView.z)));

		//Calculate the new Z of the rotation
		vNewView.z = (	(((1 - cosTheta) * fX * fZ - fY * sinTheta)	* vView.x) +
							(((1 - cosTheta) * fY * fZ + fX * sinTheta)	* vView.y) +
							(((cosTheta + (1 - cosTheta) * fZ * fZ)		* vView.z)));

		//Add the position vector with the new rotation vector
		m_pAt = m_pEye + vNewView;
	}
}

///
/// Method to move the Camera with a given speed.
///
/// @params fspeed speed to move the Camera
///
void CCamera::moveCamera(float fspeed)
{
	if(m_bCapture){
		glm::vec3 vVector = m_pAt - m_pEye;
		glm::vec3 values(vVector.x * fspeed, vVector.y * fspeed, vVector.z * fspeed);

		m_pEye += values;
		m_pAt += values;
	}
}

///
/// Method to move the Camera with a given speed.
///
/// @params fspeed speed to move the Camera
///
void CCamera::moveCamera2(float fspeed)
{
	if(m_bCapture){
		glm::vec3 vVector = m_pAt - m_pEye;
		vVector = glm::cross(vVector, m_vUp);
		float valorx = vVector.x * fspeed, valory = vVector.y * fspeed, valorz = vVector.z * fspeed;

		m_pEye.x = (m_pEye.x + valorx);
		m_pEye.y = (m_pEye.y + valory);
		m_pEye.z = (m_pEye.z + valorz);	

		m_pAt.x = (m_pAt.x + valorx);
		m_pAt.y = (m_pAt.y + valory);
		m_pAt.z = (m_pAt.z + valorz);	
	}
}

///
/// Move vertical with a given speed.
///
/// @params fspeed speed to move the Camera
///
void CCamera::moveCameraVertical(float fSpeed)
{
	if(m_bCapture)
	{
		m_pEye.y = (m_pEye.y + fSpeed);
		m_pAt.y += fSpeed;
	}
}

///
/// Method to active or desactive the capture of the mouse.
///
///
void CCamera::captureMouse()
{	
	m_bCapture = !m_bCapture;

	if(m_bCapture == true){
		//glutSetCursor(GLUT_CURSOR_NONE);
		glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}else{
		//glutSetCursor(GLUT_CURSOR_RIGHT_ARROW);
		glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

	//glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//glfwSetCursorPos(m_glfwWindow, 0.0, 0.0);