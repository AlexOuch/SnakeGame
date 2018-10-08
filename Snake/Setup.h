#include <string>
#include <iostream>
#include <sstream>

using namespace std;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"

//window resize call back function prototype
void windowResizeCallBack(GLFWwindow* window, int width, int height);

//user inputs
//void processInputs(GLFWwindow* window);

//Frames Per Second prototype
void showFPS(GLFWwindow* window);
