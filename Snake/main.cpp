//testing github
#include <string>
#include <iostream>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Setup.h"

#include "Model.h"
#include "Camera.h"

using namespace std;

bool menu = true;

//window resize call back
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

//mouse move callback
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

//wheel scroll callback
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void processInputs(GLFWwindow* window);

unsigned int loadTexture(char const * path);

//Vertex Shader Program Source Code
const char* vertexShaderSource =
"#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n" //variable location[0], read value in of type vec3
"\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\n\0";//need \0 to tell it its the end of the character string

//Fragment Shader Program Source Code
const char* fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n" //value passed on to next shader when done
"\n"
"void main()\n"
"{\n"
"FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

//Camera Details
Camera camera(glm::vec3(0.0f, 0.0f, 30.0f));
bool firstMouse = true;
float fov = 45.0f;

//My lamps position
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 lightColour(1.0f, 1.0f, 1.0f);

//mouse details
float lastX = 400, lastY = 300;

//time management
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

void main()
{

	glfwInit();
	//tell glfw that we want to work with openGL 3.3 core profile
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //the first 3 of 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //the .3 of 3.3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //core profile

	GLFWwindow *window = glfwCreateWindow(1280, 720, "Yoshi Snake", NULL, NULL);
	if (window == NULL) {
		cout << "failed to create window" << endl;
		glfwTerminate(); 
		system("pause");
		return;
	}
	glfwMakeContextCurrent(window);

	//initialise GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		//if this fails, then
		cout << "GLAD failed to initialise" << endl;
		glfwTerminate(); //cleanup glfw stuff
		system("pause");
		return;
	}

	//set up openGL viewport x,y,w,h
	glViewport(0, 0, 1280, 720);

	//hide cursor but also capture it inside this window
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//setup mouse move callback
	glfwSetCursorPosCallback(window, mouse_callback);

	//scroll wheel callback
	glfwSetScrollCallback(window, scroll_callback);


	//set z depth buffering on
	glEnable(GL_DEPTH_TEST);

	//load images in, flip them
	stbi_set_flip_vertically_on_load(true);

	glfwSetFramebufferSizeCallback(window, windowResizeCallBack);

	Shader shaderProgram1("vertexShader1.txt", "fragmentShader1.txt");
	Shader lightShader("modelShader.vs", "modelShader.fs");
	Shader lampShader("shader6.vs", "lampShader.fs");

	Model yoshiEgg("assets/YoshiEgg.obj");


	//Compile Shader Source into shader programs
	//vertex shader first
	int vertexShaderID = glCreateShader(GL_VERTEX_SHADER);//gives id for a vertex shader
														  //give it the source code
														  //params:
														  //		shader id
														  //		number of source strings
														  //		source strings (1 or array)
														  //		length of string or NULL if it ends in a \0
	glShaderSource(vertexShaderID, 1, &vertexShaderSource, NULL);
	//compile the code on the gpu
	glCompileShader(vertexShaderID);

	//check for errors
	int success;
	char infoLog[512];

	//check compile status for our program, store result in success
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
	//failed?
	if (!success) {
		//get error for the log and store in infoLog
		glGetShaderInfoLog(vertexShaderID, 512, NULL, infoLog);
		cout << "Vertex shader error!: " << infoLog << endl;
		system("pause");
	}
	//Fragment shader
	int fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderID, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShaderID);

	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
	//failed?
	if (!success) {
		//get error for the log and store in infoLog
		glGetShaderInfoLog(fragmentShaderID, 512, NULL, infoLog);
		cout << "Fragment shader error!: " << infoLog << endl;
		system("pause");
	}
	//Create a Shader Program which links a bunch of shaders together for a full pipeline
	int shaderProgramID = glCreateProgram();
	//attach our shaders to this program
	glAttachShader(shaderProgramID, vertexShaderID);
	glAttachShader(shaderProgramID, fragmentShaderID);
	//link the attached shaders
	glLinkProgram(shaderProgramID);
	//check for linking errors
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgramID, 512, NULL, infoLog);
		cout << "Shader Program Linking Error!: " << infoLog << endl;
		system("pause");
	}
	//our shaders were used to help make the full shader program, we dont need them individually anymore
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	float textureRectVertices[] = {
		// positions // colors // texture coords
		1, 1, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
		1, -1, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
		-1, -1, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
		-1, 1, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left 
	};

	unsigned int textureRectIndices[] = {
		0, 1, 3, //first triangle 
		1, 2, 3 //seconds triangle
	};

	unsigned int textureRectVBO;
	glGenBuffers(1, &textureRectVBO);
	
	unsigned int textureRectEBO;
	glGenBuffers(1, &textureRectEBO);

	unsigned int textureRectVAO;
	glGenVertexArrays(1, &textureRectVAO);
	//to work with this VAO bind it to make it the current one
	glBindVertexArray(textureRectVAO);

		//bind vbo
		glBindBuffer(GL_ARRAY_BUFFER, textureRectVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(textureRectVertices), textureRectVertices, GL_STATIC_DRAW);

		//bind ebo
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, textureRectEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(textureRectIndices), textureRectIndices, GL_STATIC_DRAW);

		//position (x, y, z)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		//colour (r, g, b)
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		//Texture Coordinate(S,T)
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

	//unbind VAO
	glBindVertexArray(0);

	unsigned int texture1ID;
	glGenTextures(1, &texture1ID);
	glBindTexture(GL_TEXTURE_2D, texture1ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//wrap on the s(x) axis
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//wraps on the t(y) axis

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//GL_LINEAR(bilinear) or GL_NEAREST for shrinking
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//for stretching

	//load up jpg file
	int width, height, numberChannels;
	unsigned char* image1Data = stbi_load("menu.jpg", &width, &height, &numberChannels, 0);

	if (image1Data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,  0, GL_RGB, GL_UNSIGNED_BYTE, image1Data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else cout << "image failed to load" << endl;
	
	stbi_image_free(image1Data);

	glfwSetCursorPos(window, lastX, lastY);
	//GAME LOOP
	while (!glfwWindowShouldClose(window)) {

		//time management
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//user input
		processInputs(window);

		glClearColor(0, 0, 1, 1); //blue
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear screen with clear colour
		
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);

		if (menu) {
			shaderProgram1.use();
			int texture1Uniform = glGetUniformLocation(shaderProgram1.ID, "texture1");
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture1ID);
			glUniform1i(texture1Uniform, 0);
			glBindVertexArray(textureRectVAO);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}

		if (!menu) {
			lightShader.use();
			lightShader.setVec3("objectColor", 1.0f, 0.5f, 1.0f);
			lightShader.setVec3("lightColor", lightColour);
			lightShader.setVec3("lightPos", lightPos);
			lightShader.setVec3("viewPos", camera.Position);

			glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
			lightShader.setMat4("model", model);
			yoshiEgg.Draw(lightShader);

			lampShader.use();
			lampShader.setVec3("lightColor", lightColour);
		}

		glfwPollEvents();
		glfwSwapBuffers(window);
		showFPS(window);

		
	}

	//optional: de-allocate all resources
	glDeleteVertexArrays(1, &textureRectVAO);//params: how many, thing with ids(unsigned int, or array of)
	glDeleteBuffers(1, &textureRectVBO);
	glDeleteBuffers(1, &textureRectEBO);
	//glDeleteBuffers(2, VBOs); //example of deleting 2 VBO ids from the VBOs array
	glfwTerminate();
	//yoshi
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void processInputs(GLFWwindow* window) {

	

	if (menu) {
		//if esc pressed, set window to 'should close'
		//if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			//glfwSetWindowShouldClose(window, true);
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			menu = false;
	}

	if (!menu) {
		//if esc pressed, set window to 'should close'
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			menu = true;

		float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, deltaTime * 5);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, deltaTime * 5);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, deltaTime * 5);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, deltaTime * 5);
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{
			lightColour.r += deltaTime;
			lightColour.g += deltaTime;
			lightColour.b += deltaTime;
			if (lightColour.r > 1)
				lightColour.r = 1;
			if (lightColour.g > 1)
				lightColour.g = 1;
			if (lightColour.b > 1)
				lightColour.b = 1;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			lightColour.r -= deltaTime;
			lightColour.g -= deltaTime;
			lightColour.b -= deltaTime;
			if (lightColour.r < 0)
				lightColour.r = 0;
			if (lightColour.g < 0)
				lightColour.g = 0;
			if (lightColour.b < 0)
				lightColour.b = 0;
		}
	}


}