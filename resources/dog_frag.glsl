#version 330 core 

uniform sampler2D Texture0;

in vec3 fragNor;
in vec2 texCoords; 

out vec4 color;  

void main()
{
	vec3 normal = normalize(fragNor.xyz);
	//vec4 texColor0 = texture(Texture0, texCoords.xy);
	//color = texColor0;

	color = vec4(0.3,0.2,0.0,1.0);

}