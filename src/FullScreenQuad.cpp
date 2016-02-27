#include "FullScreenQuad.h"
#ifdef _WIN32
	#include "GL/glew.h"
#else
	#include <GL/glew.h>
#endif
#include <stdlib.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

CFullScreenQuad::CFullScreenQuad(void) : m_iArraySize(20)
{
	/// x, y, z, u, v
	// bottom, left = (0,0) and upper, right = (1,1)
	m_vArray[0] = - 1.0f;  m_vArray[1] = -1.0f; m_vArray[2] =  0.0f;	m_vArray[3] =  -1.0f; m_vArray[4] = -1.0f;
	m_vArray[5] =   1.0f;  m_vArray[6] = -1.0f; m_vArray[7] =  0.0f;	m_vArray[8] =  1.0f; m_vArray[9] = -1.0f;
	m_vArray[10] =  1.0f; m_vArray[11] =  1.0f; m_vArray[12] = 0.0f;	m_vArray[13] = 1.0f; m_vArray[14] = 1.0f;
	m_vArray[15] = -1.0f; m_vArray[16] =  1.0f; m_vArray[17] = 0.0f;	m_vArray[18] = -1.0f; m_vArray[19] = 1.0f;
	// upper, left = (0,0) and bottom, right = (1,1)
	//m_vArray[0] = - 1.0f;  m_vArray[1] = 1.0f; m_vArray[2] =  0.0f;	m_vArray[3] =  0.0f; m_vArray[4] = 0.0f;
	//m_vArray[5] =  - 1.0f;  m_vArray[6] = -1.0f; m_vArray[7] =  0.0f;	m_vArray[8] =  1.0f; m_vArray[9] = 0.0f;
	//m_vArray[10] =  1.0f; m_vArray[11] =  -1.0f; m_vArray[12] = 0.0f;	m_vArray[13] = 1.0f; m_vArray[14] = 1.0f;
	//m_vArray[15] = 1.0f; m_vArray[16] =  1.0f; m_vArray[17] = 0.0f;	m_vArray[18] = 0.0f; m_vArray[19] = 1.0f;

}


CFullScreenQuad::~CFullScreenQuad(void)
{
}

void CFullScreenQuad::initialize()
{
	glGenBuffers(1, &m_uVBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_uVBO);
	glBufferData(GL_ARRAY_BUFFER, m_iArraySize * sizeof(float), &m_vArray, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CFullScreenQuad::draw()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_uVBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, BUFFER_OFFSET(0));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, BUFFER_OFFSET(sizeof(float)*3));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}
//https://www.opengl.org/discussion_boards/showthread.php/170137-GL_TRIANGLE_FAN-versus-GL_QUADS-performance
