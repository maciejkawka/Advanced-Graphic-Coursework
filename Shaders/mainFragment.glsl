#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;
uniform sampler2D emissionTex;

vec4 fogColour = vec4(0.2,0.2,0.2,1);
float density = 0.007;
uniform int useFog;
uniform int useTex;
uniform int useBump;

in Vertex{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	vec4 eyeDir;
} IN;

out vec4 fragColour[4];

void main(void){

	mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));

	vec3 normal = texture2D(bumpTex, IN.texCoord).rgb*2.0-1.0;
	normal = normalize(TBN*normalize(normal));
	
	float fog =1.0;

	if(useFog==1)
	{
		float fogCoords = abs(IN.eyeDir.z/IN.eyeDir.w);
		fog = exp(-density*fogCoords);
	}

	fog = 1.0-clamp(fog,0,1);
	vec4 finalFog = fogColour * fog;
	finalFog.a = 1.0;


	if(useBump==0)
		normal=IN.normal;

	if(useTex==1)
		fragColour[0] = texture2D(diffuseTex, IN.texCoord);
	else
		fragColour[0]= IN.colour;
	

	fragColour[1] = vec4(normal.xyz *0.5+0.5,1.0);

	vec4 emission = texture2D(emissionTex,IN.texCoord);
	fragColour[2]= emission*IN.colour;
	fragColour[3] = finalFog;
}