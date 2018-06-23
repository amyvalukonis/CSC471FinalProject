#version  330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform float uTime;
 
out vec3 fragNor;
out vec2 texCoords;  



void main()
{
	vec3 position = vec3(vertPos);
	gl_Position = P * V * M * vec4(position,1.0);
	fragNor = (V * M * vec4(vertNor, 0.0)).xyz;
	texCoords = vertTex;
}