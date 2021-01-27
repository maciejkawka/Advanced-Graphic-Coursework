#version 330 core

uniform samplerCube cubeTex;

in Vertex{
	vec3 viewDir;
} IN;

out vec4 fragColour;
out vec4 diffuseOutput;

void main(void){
	fragColour = texture(cubeTex, normalize(IN.viewDir));
	diffuseOutput = texture(cubeTex, normalize(IN.viewDir));
}