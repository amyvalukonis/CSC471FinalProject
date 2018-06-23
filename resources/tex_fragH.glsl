#version 330 core

in vec2 texCoord;

//extract two images from a rendered scene in a single render pass 
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;  

uniform sampler2D texBuf;

uniform vec2 dir;


void main(){
	//FragColor = vec4(texture(texBuf,texCoord).rgb,1.0); 
	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 0.0) {
		BrightColor = vec4(FragColor.rgb,1.0); 
	}
	else {
		BrightColor = vec4(0.0,0.0,0.0,1.0);
	}
}