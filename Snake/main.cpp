//testing github
#include <string>
#include <iostream>
#include <sstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Shader.h"
#include "Setup.h"

using namespace std;

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

	//set z depth buffering on
	//glEnable(GL_DEPTH_TEST);

	//load images in, flip them
	stbi_set_flip_vertically_on_load(true);

	glfwSetFramebufferSizeCallback(window, windowResizeCallBack);

	Shader shaderProgram1("vertexShader1.txt", "fragmentShader1.txt");

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

	
	//GAME LOOP
	while (!glfwWindowShouldClose(window)) {

		//user input
		processInputs(window);

		glClearColor(0, 0, 1, 1); //blue
		glClear(GL_COLOR_BUFFER_BIT); //clear screen with clear colour

		shaderProgram1.use();
		int texture1Uniform = glGetUniformLocation(shaderProgram1.ID, "texture1");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1ID);
		glUniform1i(texture1Uniform, 0);
		glBindVertexArray(textureRectVAO);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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


