#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

class Shader
{
public:
	//id of shaderProgram (the one with the smaller shaders attached to it)
	unsigned int ID;

	Shader(const char* vertexPath, const char* fragmentPath);
	void use();
private:
	void checkCompileErrors(unsigned int shaderID, string type);



};
