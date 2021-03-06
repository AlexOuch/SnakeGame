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

//movement of models
float posX;
float posZ;
bool movingUp;
bool movingDown;
bool movingLeft;
bool movingRight;
float yoshiRotation = glm::radians(-90.0f);
void resetMovement();

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
	Shader groundShader("cubeVertexShader.txt", "cubeFragmentShader.txt");

	Model yoshiEgg("assets/Egg/YoshiEgg.obj");
	Model yoshi("assets/Yoshi/Yoshi.obj");

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

	float textureCubeVertices[] = {
		//x		y		z	tenX	tenY
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, -1.0f,
		-0.5f, -0.5f, -0.5f, 1.0f, -1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, -1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f,

		0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, -1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, -1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f
	};

	//menu vbo/vao and binding
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

	//menu
	int width, height, numberChannels;
	unsigned char* image1Data = stbi_load("menu.jpg", &width, &height, &numberChannels, 0);

	if (image1Data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,  0, GL_RGB, GL_UNSIGNED_BYTE, image1Data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else cout << "image failed to load" << endl;
	
	stbi_image_free(image1Data);





	//generate a texture in gpu, return id
	unsigned int cubeTexture1ID;
	glGenTextures(1, &cubeTexture1ID);
	//we bind the texture to make it the one we're working on
	glBindTexture(GL_TEXTURE_2D, cubeTexture1ID);
	//set wrapping options(repeat texture if texture coordinates dont fully cover polygons)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//wrap on the s(x) axis
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//wraps on the t(y) axis
																 //set filtering options
																 //Suggestion use nearest neighbour for pixel art, use bilinear for pretty much everything else
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//GL_LINEAR(bilinear) or GL_NEAREST for shrinking
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//for stretching

																	 //LOAD UP IMAGE FILE (JPEG FIRST)
	int cubeWidth, cubeHeight, cubeNumberChannels; //as we load an image, we'll get values from it to fill these in
	unsigned char *ground1Data = stbi_load("assets/yoshiGround/yoshiGround.jpg", &cubeWidth, &cubeHeight, &cubeNumberChannels, 0);
	//if it loaded
	if (ground1Data) {
		cout << "Success! Image is " << cubeWidth << " by " << cubeHeight << "pixels" << endl;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cubeWidth, cubeHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, ground1Data);

		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		cout << "Image load failed!" << endl;
	}
	//cleanup image memory
	stbi_image_free(ground1Data);

	//Generate a texture in our graphics card to work with
	unsigned int cubeTexture2ID;
	glGenTextures(1, &cubeTexture2ID); //generate 1 texture id and store in texture2ID
	glBindTexture(GL_TEXTURE_2D, cubeTexture2ID);//make this texture the currently working texture, sayings its a 2d texture (as opposed to 1d and 3d)
											 //how will texture repeat on large surfaces
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);//how wrap horizontally (S axis...)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//how to wrap vertically (T axis..)
																 //how will texture deal with shrink and stretch
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //when shrinking texture use bilinear filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//use nearest neighbour filtering on stretch

																	  //Load up an image
	unsigned char *groundData2 = stbi_load("assets/yoshiGround/yoshiGround.png", &cubeWidth, &cubeHeight, &cubeNumberChannels, 0);
	if (groundData2) {
		//give the texture in our graphics card the data from this png file
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cubeWidth, cubeHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, groundData2);
		//generate mipmaps for this texture
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		cout << "failed to load score" << endl;
	}
	//free image data from ram because theres a copy in the texture
	stbi_image_free(groundData2);

	unsigned int cubeVBO;
	glGenBuffers(1, &cubeVBO);

	//3. Vertex Array Object? tries to describe the data in the VBO and relay it to the first shader
	unsigned int cubeVAO;
	glGenVertexArrays(1, &cubeVAO);

	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCubeVertices), textureCubeVertices, GL_STATIC_DRAW);

	//xyz to location = 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//texture coordinates to location = 1
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//unbind stuff
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);





	glfwSetCursorPos(window, lastX, lastY);
	//GAME LOOP
	//camera position and angle
	camera.setPosition(0, 50.0f, 30.0f);
	camera.setAngle(-90.0f, -50.0f);

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


			//resetting yoshi and camera
			camera.setPosition(0, 50.0f, 30.0f);
			camera.setAngle(-90.0f, -50.0f);

			posX = 0;
			posZ = 0;
			resetMovement();
			yoshiRotation = glm::radians(-90.0f);
			//
			
			//menu screen			
			shaderProgram1.use();
			int texture1Uniform = glGetUniformLocation(shaderProgram1.ID, "texture1");
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture1ID);
			glUniform1i(texture1Uniform, 0);
			glBindVertexArray(textureRectVAO);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}

		if (!menu) {

			groundShader.use();

			glBindVertexArray(cubeVAO);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, cubeTexture1ID);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, cubeTexture2ID);

			glUniform1i(glGetUniformLocation(groundShader.ID, "texture1"), 0);
			glUniform1i(glGetUniformLocation(groundShader.ID, "texture2"), 1);

			glUniformMatrix4fv(glGetUniformLocation(groundShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(groundShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

			glm::mat4 ground = glm::mat4(1.0f);
			ground = glm::translate(ground, glm::vec3(0.0f, -40.0f, -25.0f));
			ground = glm::scale(ground, glm::vec3(80.0f, 80.0f, 80.0f));

			glUniformMatrix4fv(glGetUniformLocation(groundShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(ground));
			glDrawArrays(GL_TRIANGLES, 0, 36); //strarting at stride0, draw 36 rows of vertex data

			//model stuff
			lightShader.use();
			lightShader.setVec3("objectColor", 1.0f, 0.5f, 1.0f);
			lightShader.setVec3("lightColor", lightColour);
			lightShader.setVec3("lightPos", lightPos);
			lightShader.setVec3("viewPos", camera.Position);

			glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

			//eggmodel
			glm::mat4 eggModel = glm::mat4(1.0f);
			eggModel = glm::translate(eggModel, glm::vec3(4.0f, 0.0f, 0.0f));
			eggModel = glm::scale(eggModel, glm::vec3(0.03f, 0.03f, 0.03f));
			lightShader.setMat4("model", eggModel);
			yoshiEgg.Draw(lightShader);
			
			//yoshi model
			glm::mat4 yoshiModel = glm::mat4(1.0f);
			yoshiModel = glm::translate(yoshiModel, glm::vec3(posX, 0.0f, posZ));
			yoshiModel = glm::rotate(yoshiModel, yoshiRotation, glm::vec3(0, 1, 0));
			yoshiModel = glm::scale(yoshiModel, glm::vec3(10.0f, 10.0f, 10.0f));
			lightShader.setMat4("model", yoshiModel);
			yoshi.Draw(lightShader);

			//movement
			if (movingUp) {
				posZ -= deltaTime * 30;
				if (posZ < -65)
					posZ = -65;
			}
			if (movingDown) {
				posZ += deltaTime * 30;
				if (posZ > 15)
					posZ = 15;
			}
			if (movingLeft) {
				posX -= deltaTime * 30;
				if (posX < -40)
					posX = -40;
			}
			if (movingRight) {
				posX += deltaTime * 30;
				if (posX > 40)
					posX = 40;
			}
			//lampShader.use();
			//lampShader.setVec3("lightColor", lightColour);
			

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
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			menu = false;
	}

	if (!menu) {
		if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS)
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
		


		//yoshi movement
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			yoshiRotation = glm::radians(180.0f);
			resetMovement();
			movingUp = true;

		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			yoshiRotation = glm::radians(0.0f);
			resetMovement();
			movingDown = true;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			yoshiRotation = glm::radians(-90.0f);
			resetMovement();
			movingLeft = true;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			yoshiRotation = glm::radians(90.0f);
			resetMovement();
			movingRight = true;
		}
	}
}

void resetMovement() {
	movingUp = false;
	movingDown = false;
	movingLeft = false;
	movingRight = false;
}