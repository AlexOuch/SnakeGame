#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;
in vec3 Normal; 
in vec3 FragPos; 

//uniform sampler2D ourTexture;
uniform sampler2D texture_diffuse1; //default to texture0 bind, change which they are a bound to via code
uniform sampler2D texture_specular1; //default to texture0 bind

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
 
uniform vec3 viewPos;

void main()
{
	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor * vec3(texture(texture_diffuse1, TexCoord));
	
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);  
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor * vec3(texture(texture_diffuse1, TexCoord));
	
	float specularStrength = 1;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm); 
	//                                           specular to the power of 32, the higher the number, the more pinpointed and brighter the light is
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 64);
	vec3 specular = spec * lightColor * vec3(texture(texture_specular1, TexCoord));  

    vec3 result = (ambient + diffuse + specular) ;//* objectColor;
    FragColor = vec4(result, 1.0);
	//FragColor = vec4(lightColor * objectColor, 1.0);//colour based on coloured light reflection
    //FragColor = mix(texture(texture_diffuse1, TexCoord), texture(texture2, TexCoord), 0.2);
	//FragColor = texture(texture2, TexCoord);//* vec4(ourColor, 1.0);
}