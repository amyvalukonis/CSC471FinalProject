#version 330 core 

out vec4 FragColor; 
in vec3 TexCoords; 

uniform samplerCube skybox; 
uniform samplerCube skybox2; 
//uniform float blendFactor; 

uniform vec3 fogColor; 

uniform int scene;

float lowerLimit = 0.0; 
float upperLimit = 0.3; 

void main()
{
	vec4 finalColor;
	if(scene == 1) {
		finalColor = texture(skybox, TexCoords); 
	}
	else {
		finalColor = texture(skybox2, TexCoords);
	}
	//vec4 texture1 = texture(skybox, TexCoords); 
	//vec4 texture2 = texture(skybox2, TexCoords); 

	//vec4 finalColor = mix(texture1,texture2, blendFactor);

	//vec4 finalColor = texture(skybox, TexCoords); 
	
	float factor = (TexCoords.y-lowerLimit) / (upperLimit - lowerLimit);
	factor = clamp(factor,0.0,1.0);
	FragColor = mix(vec4(fogColor,1.0), finalColor, factor); 
	//FragColor = texture(skybox,TexCoords);
}

