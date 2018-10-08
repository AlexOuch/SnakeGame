#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;
in vec3 Normal; 
in vec3 FragPos; 

//uniform sampler2D ourTexture;
uniform sampler2D texture1; //default to texture0 bind, change which they are a bound to via code
uniform sampler2D texture2; //default to texture0 bind

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos; 

void main()
{
	

	FragColor = vec4(lightColor, 1.0); 
	
	
}