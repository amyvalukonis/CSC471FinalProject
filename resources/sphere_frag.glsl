#version 330 core 
in vec3 fragNor;
out vec4 color; 

in vec4 vertPosOut; 
in vec3 vertNorOut; 
in mat4 MOut; 
in mat4 POut; 
in mat4 VOut; 

uniform int white; 
void main()
{
	vec3 light;
	vec3 normal = normalize(fragNor.xyz);

	vec3 L = vec3(normalize(vec3(3,6,12)));
	vec3 diffuse = vec3(0.9,0.9,0.9); 
	vec3 ambient = vec3(0.2,0.2,0.2); 

	if(white==1) {
		light = vec3(1.0,1.0,1.0); 
	}
	else {
		light = vec3(0.9,0.7,0.5);
	}
	//vec3 red = vec3(1.0,0.0,0.0);

	color = vec4(vec3(((diffuse*light) * clamp(dot(normal, L), 0.0, 1.0)) + ambient*light),1.0);
}