#define GLFW_DLL

#include "GLSLProgram.h"
#include "Application.h"
#include <stdlib.h>
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
	CApplication& gApp = CApplication::getInstance();
	if(!gApp.initialize())
	{
		gApp.destroy();
		exit(EXIT_FAILURE);
	}
	
	double lastTime = glfwGetTime();
	double difference;
	int nbFrames = 0;
	while(!gApp.closeApp())
	{
		nbFrames++;
		difference = glfwGetTime() - lastTime;
		gApp.update(difference);
		if (difference >= 1.0 )// If last prinf() was more than 1 sec ago
		{
			// printf and reset timer
			//printf("%f ms/frame (FPS:%d)\n", 1000.0/double(nbFrames), nbFrames);
			nbFrames = 0;
			lastTime += 1.0;
    }
		gApp.run();
	}
	gApp.destroy();
	return EXIT_SUCCESS;
}
