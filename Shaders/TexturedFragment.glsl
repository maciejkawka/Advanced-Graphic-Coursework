#version 330 core
uniform sampler2D diffuseTex;


in Vertex{
	vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void)
{
	vec4 colour = texture(diffuseTex,IN.texCoord);
	
	fragColour=colour;
	
}