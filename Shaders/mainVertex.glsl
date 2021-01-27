#version 330 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec4 nodeColour;
uniform mat4 textureMatrix;

in vec3 position;
in vec3 normal;
in vec4 tangent;
in vec2 texCoord;

out Vertex{
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	vec4 eyeDir;
} OUT;


void main(void)	
{
	OUT.colour = nodeColour;
	OUT.texCoord = (textureMatrix*vec4(texCoord,0.0,1.0)).xy;

	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

	vec3 wNormal = normalize(normalMatrix * normalize(normal));
	vec3 wTangent = normalize(normalMatrix * normalize(tangent.xyz));

	OUT.normal = wNormal;
	OUT.tangent = wTangent;
	OUT.binormal = cross(wTangent, wNormal)*tangent.w;

	vec4 worldPos = (modelMatrix* vec4(position,1));
	OUT.worldPos = worldPos.xyz;
	OUT.eyeDir = (viewMatrix * worldPos);
	gl_Position = (projMatrix*viewMatrix)*worldPos;

}
