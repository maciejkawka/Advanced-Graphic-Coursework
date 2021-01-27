#version 330 core
uniform sampler2D diffuseTex;
uniform sampler2D diffuseLight;
uniform sampler2D specularLight;
uniform sampler2D emissionTex;
uniform sampler2D fogTex;
in Vertex{
	vec2 texCoord;
}IN;

out vec4 fragColour;

void main(void){
	vec3 diffuse = texture(diffuseTex, IN.texCoord).xyz;
	vec3 light = texture(diffuseLight, IN.texCoord).xyz;
	vec3 specular = texture(specularLight, IN.texCoord).xyz;
	vec4 emission = texture(emissionTex, IN.texCoord);
	vec4 fog = texture(fogTex, IN.texCoord);

	fragColour.xyz = diffuse*0.1;
	fragColour.xyz +=diffuse*light;
	fragColour.xyz +=specular;
	fragColour += emission;
	fragColour += fog;
	fragColour.a = 1.0;
}
