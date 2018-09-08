#include "Setup.h"

//window resize call back function prototype
void windowResizeCallBack(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}


bool wireFrame;
//user inputs
void processInputs(GLFWwindow* window) {

	//if esc pressed, set window to 'should close'
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		//flip wiremode value
		wireFrame = !wireFrame;
		if (wireFrame) //if(wireframe == true)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	//if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	//	selectedColour = 1;
}

void showFPS(GLFWwindow* window) {
	//static function variables are declared 
	//once per project run, so these 2 lines of 
	//code run once and then the variables persist
	//until the end of the program
	static double previousSeconds = 0.0;
	static int frameCount = 0;
	double elapsedSeconds;
	double currentSeconds = glfwGetTime();
	//getTime returns seconds since startup

	elapsedSeconds = currentSeconds - previousSeconds;
	if (elapsedSeconds > 0.25) {
		previousSeconds = currentSeconds;
		double fps = frameCount / elapsedSeconds;
		double msPerFrame = 1000.0 / fps;

		stringstream ss;
		ss.precision(3);//3 decimal places
		ss << fixed << "Yoshi Snake FPS: " << fps << " Frame Time: " << msPerFrame << "(ms)";

		glfwSetWindowTitle(window, ss.str().c_str());
		frameCount = 0;
	}
	frameCount++;
}