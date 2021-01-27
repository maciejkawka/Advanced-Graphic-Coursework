#pragma once
#include "../NCLGL/OGLRenderer.h"

class HeightMap;
class Camera;
class Light;
class SceneNode;
class MeshAnimation;
class MeshMaterial;
class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);
	void RenderScene()				override;
	void UpdateScene(float msec)	override;
	void AutoCameraMovement();
	inline void EnableFog() 
	{
		if (useFog == 1)
			useFog = 0;
		else
			useFog = 1;
	}
	inline void EnableBlur() { useBlur = !useBlur; }
protected:

	void GenerateSceneTexture(GLuint& into, bool depth = false);
	void DrawNode(SceneNode* n, Shader* shader);
	void DrawAnimation(Mesh* m, MeshAnimation* meshAnimation, vector<GLuint>& textures);
	

	//RENDER FLOW
	void DrawShadows();
	void DrawMainScene();
	void DrawLights();
	void CombineBffers();
	void PostProcess();
	void PresentScene();

	//SHADERS
	Shader* shadowShader;
	Shader* mainShader;
	Shader* lightShader;
	Shader* combineShader;
	Shader* postProcesShader;
	Shader* presentShader;
	Shader* skyboxShader;
	Shader* animationShader;

	//FBO TEXTURES
	GLuint shadowTex;
	GLuint bufferColourTex;
	GLuint bufferNormalTex;
	GLuint bufferDepthTex;
	GLuint lightDiffuseTex;
	GLuint lightSpecularTex;
	GLuint processDepthTex;
	GLuint processColourTex[2];
	GLuint emissionTex;
	GLuint fogTex;

	//TEXTURES
	vector<GLuint> texture;
	vector<GLuint> normalTex;
	vector<GLuint> emitTex;
	vector<GLuint> robotTextures;

	//FRAMEBUFFERS
	GLuint shadowFBO;
	GLuint mainFBO;
	GLuint lightFBO;
	GLuint combineFBO;
	GLuint processFBO;

	//TEST
	Mesh* sphere;
	Mesh* quad;	
	Light* light;
	Light* shadowLight;
	HeightMap* heightMap;
	vector<SceneNode*> objects;
	SceneNode* root;
	Camera* camera;
	SceneNode* lightObjects;

	//Animation
	int currentFrame;
	float frameTime;
	Mesh* robot;
	MeshAnimation* animation;
	MeshMaterial* material;
	float sceneTime;
	Matrix4 robotPosition;

	//Objects Movment
	bool cameraMove;
	Vector3 robotMove;

	//Fog
	int useFog;

	//Blur
	bool useBlur;
	
};
