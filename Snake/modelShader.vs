#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;
layout (location = 1) in vec3 aNormal;


out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos; 

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
// note that we read the multiplication from right to left (matrix multiplication rule)
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    TexCoord = aTexCoord;
	Normal = mat3(transpose(inverse(model))) * aNormal; ; //inversing matrices is expensive, try to do outside of shader where possible
	FragPos = vec3(model * vec4(aPos, 1.0));//frag in world space, not based on camera
}