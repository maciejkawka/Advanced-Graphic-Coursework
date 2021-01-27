#include "Renderer.h"
#include"../nclgl/Camera.h"
#include"../nclgl/HeightMap.h"
#include"..//nclgl/Light.h"
#include"../nclgl/SceneNode.h"
#include"../nclgl/MeshAnimation.h"
#include"../nclgl/MeshMaterial.h"

#define SHADOWDIR "../Corsework/"
#define SHADOWSIZE  2048*4
#define LIGHT_NUM 10
int POST_PASSES = 0;


Renderer::Renderer(Window& parent) : OGLRenderer(parent)
{


	//TEST
	GLuint earthTex = SOIL_load_OGL_texture(TEXTUREDIR"Rock.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	GLuint earthBump = SOIL_load_OGL_texture(TEXTUREDIR"RockBump.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	GLuint cubeTex = SOIL_load_OGL_texture(TEXTUREDIR"cubeTex.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	GLuint cubeBump = SOIL_load_OGL_texture(TEXTUREDIR"CubeBump.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	GLuint cubeEmission = SOIL_load_OGL_texture(TEXTUREDIR"CubeEmission.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	GLuint cubeEmission1 = SOIL_load_OGL_texture(TEXTUREDIR"CubeEmission1.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	GLuint windowEmission = SOIL_load_OGL_texture(TEXTUREDIR"windowEmission.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	GLuint glowEmission = SOIL_load_OGL_texture(TEXTUREDIR"white.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	GLuint waterTex = SOIL_load_OGL_texture(TEXTUREDIR"water.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	GLuint waterBump = SOIL_load_OGL_texture(TEXTUREDIR"waterbump.PNG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	GLuint BuildingTex = SOIL_load_OGL_texture(TEXTUREDIR"BuildingTex.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	GLuint cubeMap = SOIL_load_OGL_cubemap(TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg",
		TEXTUREDIR"rusted_up.jpg", TEXTUREDIR"rusted_down.jpg",
		TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg",
		SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0);


	texture.push_back(earthTex);
	texture.push_back(cubeTex);
	texture.push_back(BuildingTex);
	texture.push_back(waterTex);
	texture.push_back(cubeMap);

	normalTex.push_back(earthBump);
	normalTex.push_back(cubeBump);
	normalTex.push_back(waterBump);


	emitTex.push_back(cubeEmission);
	emitTex.push_back(cubeEmission1);
	emitTex.push_back(glowEmission);
	emitTex.push_back(windowEmission);


	for (int i = 0; i < texture.size(); ++i)
		SetTextureRepeating(texture[i], true);

	for (int i = 0; i < normalTex.size(); ++i)
		SetTextureRepeating(normalTex[i], true);

	for (int i = 0; i < emitTex.size(); ++i)
		SetTextureRepeating(emitTex[i], true);

	lightObjects = new SceneNode();
	root = new SceneNode();
	quad = Mesh::GenerateQuad();
	heightMap = new HeightMap(TEXTUREDIR"customHeightmap.png");
	Vector3 heightmapSize = heightMap->GetHeightmapSize();
	sphere = Mesh::LoadFromMeshFile("Sphere.msh");
	SceneNode* n;

	//HIGH MAP
	n = new SceneNode();
	n->SetMesh(heightMap);
	n->SetTexture(earthTex);
	n->SetNormalTexture(earthBump);
	root->AddChild(n);

	//City
	SceneNode* city = new SceneNode();
	city->SetTransform(Matrix4::Translation(heightmapSize * Vector3(0.0f, 1.1f, 0.0f)));
	root->AddChild(city);


	//Buildings
	for (int j = 0; j < 4; j++)
	{
		for (int i = 0; i < 8; i++)
		{
			int switcher = rand()%2;
			GLuint texture = cubeEmission1;
			if (switcher == 1)
				texture = windowEmission;
			
			n = new SceneNode();
			n->SetMesh(Mesh::LoadFromMeshFile("Cube.msh"));
			n->SetColour(Vector4(0.4f + (float)(rand() / (float)RAND_MAX),
				0.4f + (float)(rand() / (float)RAND_MAX),
				0.4f + (float)(rand() / (float)RAND_MAX),
				1));
			n->SetTransform(Matrix4::Translation(Vector3(200 * j, 0, rand()%120+rand()%150+200 * i)));
			n->SetModelScale(Vector3(100 + rand() % 50, 200 + rand() % 400, 100 + rand() % 50));
			n->SetEmissionTexture(texture);
			n->SetTextureSize(rand() % 5 + 5);
			city->AddChild(n);
		}
	}
	//Dark buildings
	for (int i = 0; i < 6; i++)
	{
		int switcher = rand() % 2;
		GLuint texture = cubeEmission1;
		if (switcher == 1)
			texture = windowEmission;

		n = new SceneNode();
		n->SetMesh(Mesh::LoadFromMeshFile("Building.msh"));
		n->SetTransform(Matrix4::Translation(Vector3(-300, 0, rand() % 120 + rand() % 150 + 300 * i)) * Matrix4::Rotation(90,Vector3(0,1,0)));
		n->SetModelScale(Vector3(8,12,8));
		n->SetTexture(BuildingTex);
		city->AddChild(n);

	}


	//Room
	SceneNode* Room = new SceneNode();
	Room->SetTransform(Matrix4::Translation(heightmapSize * Vector3(0.85f, 1.0f, 0.4f)));
	root->AddChild(Room);

	//Cube Wall 
	int roomWidth = 350;
	int startingPoint = -10;
	for (int j = 0; j < 2; ++j)
	{
		for (int i = startingPoint; i < 10; i++)
		{
			n = new SceneNode(Mesh::LoadFromMeshFile("Cube.msh"));
			n->SetTransform(Matrix4::Translation(Vector3(35 * i, 0, j * roomWidth)));
			n->SetModelScale(Vector3(30, 30, 30));
			n->SetColour(Vector4(1, 0, 1, 1));
			n->SetTexture(cubeTex);
			n->SetNormalTexture(cubeBump);
			n->SetEmissionTexture(cubeEmission);
			Room->AddChild(n);

		}

		for (int i = startingPoint; i < 10; i++)
		{
			n = new SceneNode(Mesh::LoadFromMeshFile("Cube.msh"));
			n->SetTransform(Matrix4::Translation(Vector3(35 * i, 35, j * roomWidth)));
			n->SetModelScale(Vector3(30, 30, 30));
			n->SetColour(Vector4(0, 1, 1, 1));
			n->SetTexture(cubeTex);
			n->SetNormalTexture(cubeBump);
			n->SetEmissionTexture(cubeEmission);
			Room->AddChild(n);

		}

		for (int i = startingPoint; i < 10; i++)
		{
			n = new SceneNode(Mesh::LoadFromMeshFile("Cube.msh"));
			n->SetTransform(Matrix4::Translation(Vector3(35 * i, 70, j * roomWidth)));
			n->SetModelScale(Vector3(30, 30, 30));
			n->SetColour(Vector4(0, 1, 0, 1));
			n->SetTexture(cubeTex);
			n->SetNormalTexture(cubeBump);
			n->SetEmissionTexture(cubeEmission);
			Room->AddChild(n);

		}

		for (int i = startingPoint; i < 10; i++)
		{
			n = new SceneNode(Mesh::LoadFromMeshFile("Cube.msh"));
			n->SetTransform(Matrix4::Translation(Vector3(35 * i, 70, j * roomWidth)));
			n->SetModelScale(Vector3(30, 30, 30));
			n->SetColour(Vector4(0, 1, 0, 1));
			n->SetTexture(cubeTex);
			n->SetNormalTexture(cubeBump);
			n->SetEmissionTexture(cubeEmission);
			Room->AddChild(n);

		}


		for (int i = startingPoint; i < 10; i++)
		{
			n = new SceneNode(Mesh::LoadFromMeshFile("Cube.msh"));
			n->SetTransform(Matrix4::Translation(Vector3(35 * i, 105, j * roomWidth)));
			n->SetModelScale(Vector3(30, 30, 30));
			n->SetColour(Vector4(1, 0, 0, 1));
			n->SetTexture(cubeTex);
			n->SetNormalTexture(cubeBump);
			n->SetEmissionTexture(cubeEmission);
			Room->AddChild(n);

		}
	}
	
	//Wall
	n = new SceneNode();
	n->SetMesh(Mesh::GenerateQuad());
	n->SetTransform(Matrix4::Translation(Vector3(350, 0, 170)) * Matrix4::Rotation(-90, Vector3(0, 1, 0)));
	n->SetModelScale(Vector3(200, 130, -1));
	n->SetTexture(cubeTex);
	n->SetNormalTexture(cubeBump);
	n->SetEmissionTexture(cubeEmission1);
	n->SetColour(Vector4(0.8, 0.8, 0.8, 1));
	n->SetTextureSize(3);
	Room->AddChild(n);

	//Floor
	n = new SceneNode();
	n->SetMesh(Mesh::GenerateQuad());
	n->SetTransform(Matrix4::Translation(Vector3(-3, -15, 170)) * Matrix4::Rotation(-90, Vector3(0, 1,0 ))* Matrix4::Rotation(-90, Vector3(1, 0, 0 )));
	n->SetModelScale(Vector3(200, 400, -1));
	n->SetTexture(cubeTex);
	n->SetNormalTexture(cubeBump);
	n->SetEmissionTexture(cubeEmission1);
	n->SetColour(Vector4(0.8, 0.8, 0.8, 1));
	n->SetTextureSize(2);
	Room->AddChild(n);

	//ShadowingLight
	n = new SceneNode();
	n->SetMesh(Mesh::LoadFromMeshFile("Sphere.msh"));
	n->SetTransform(Matrix4::Translation(Vector3(350, 150, 170)));
	n->SetModelScale(Vector3(10, 10, 10));
	n->SetColour(Vector4(1, 1, 1, 1));
	n->SetEmissionTexture(glowEmission);
	Room->AddChild(n);

	//Flying Cubes
	for (int i = 0; i < 4; i++)
	{
		n = new SceneNode(Mesh::LoadFromMeshFile("Cube.msh"));
		n->SetTransform(Matrix4::Translation(Vector3(105, 40, 70+ 70*i)));
		n->SetModelScale(Vector3(30, 30, 30));
		n->SetColour(Vector4(1, 0.5, 0, 1));
		n->SetTexture(cubeTex);
		n->SetNormalTexture(cubeBump);
		n->SetEmissionTexture(cubeEmission);
		Room->AddChild(n);
	}

	//Robot
	robot = Mesh::LoadFromMeshFile("Role_T.msh");
	animation = new MeshAnimation("Role_T.anm");
	material = new MeshMaterial("Role_T.mat");
	robotPosition = Room->GetTransform()*Matrix4::Translation(Vector3(0,-15,50));
	for (int i = 0; i < robot->GetSubMeshCount(); ++i)
	{
		const MeshMaterialEntry* matEntry = material->GetMaterialForLayer(i);
		const string* filename = nullptr;
		matEntry->GetEntry("Diffuse", &filename);
		string path = TEXTUREDIR + *filename;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		robotTextures.emplace_back(texID);
	}


	//Water
	n = new SceneNode(Mesh::GenerateQuad());
	n->SetTransform(Matrix4::Translation(heightmapSize* Vector3(0.5, 0.8f, 0.5))* Matrix4::Rotation(-90, Vector3(1, 0, 0)));
	n->SetModelScale(Vector3(heightmapSize.x/2, heightmapSize.z/2, 1));
	n->SetTexture(waterTex);
	n->SetNormalTexture(waterBump);
	root->AddChild(n);

	///CUBEMAP
	skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	//Camera
	camera = new Camera(-0.0f, -90.0f, heightmapSize * Vector3(0.75f, 1.2f, 0.50f));
	cameraMove = false;

	
	//Lights
	light = new Light[LIGHT_NUM];

	for (int i = 0; i < LIGHT_NUM; ++i)
	{
		Light& l = light[i];


		l.SetPosition(Vector3(heightmapSize*Vector3(0.1,1.5,0.1+0.2*i)));

		l.SetColour(Vector4(0.1f + (float)(rand() / (float)RAND_MAX),
			0.1f + (float)(rand() / (float)RAND_MAX),
			0.1f + (float)(rand() / (float)RAND_MAX),
			1));

		l.SetRadius(500.0f);
	}

	shadowLight = new Light();
	shadowLight->SetColour(Vector4(0.5, 0.5, 0.5, 1));
	shadowLight->SetPosition(Vector3(200, 150, 170)+ heightmapSize * Vector3(0.85f, 1.0f, 0.4f));
	shadowLight->SetRadius(400.0f);



	//DRAW SHADER
	shadowShader = new Shader("shadowVert.glsl", "shadowFrag.glsl");

	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !shadowTex)
		return;

	// DRAW MAINSCENE
	animationShader = new Shader("animationVertex.glsl", "mainFragment.glsl");
	mainShader = new Shader("mainVertex.glsl", "mainFragment.glsl");
	GLenum buffers[4] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};

	GenerateSceneTexture(bufferDepthTex, true);
	GenerateSceneTexture(bufferColourTex);
	GenerateSceneTexture(bufferNormalTex);
	GenerateSceneTexture(emissionTex);
	GenerateSceneTexture(fogTex);

	glGenFramebuffers(1, &mainFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bufferColourTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bufferNormalTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, emissionTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, fogTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, bufferDepthTex, 0);
	glDrawBuffers(4, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !bufferColourTex || !bufferNormalTex || !bufferDepthTex||!emissionTex||!fogTex)
		return;

	//DRAW LIGHT
	lightShader = new Shader("lightVertex.glsl", "lightFragment.glsl");

	GenerateSceneTexture(lightDiffuseTex);
	GenerateSceneTexture(lightSpecularTex);

	glGenFramebuffers(1, &lightFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightDiffuseTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, lightSpecularTex, 0);
	glDrawBuffers(2, buffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !lightDiffuseTex || !lightSpecularTex)
		return;

	//COMBINE BUFFERS
	combineShader = new Shader("combinevert.glsl", "combinefrag.glsl");

	glGenTextures(1, &processDepthTex);
	glBindTexture(GL_TEXTURE_2D, processDepthTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	for (int i = 0; i < 2; i++)
		GenerateSceneTexture(processColourTex[i]);

	glGenFramebuffers(1, &combineFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, combineFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, processDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, processDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, processColourTex[0], 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !processDepthTex || !processColourTex[0])
		return;

	//POSTPROCES
	postProcesShader = new Shader("TexturedVertex.glsl", "processfrag.glsl");
	glGenFramebuffers(1, &processFBO);

	//Present Scene
	presentShader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");


	if (!shadowShader->LoadSuccess() || !mainShader->LoadSuccess() || !lightShader->LoadSuccess() || !combineShader->LoadSuccess() || !postProcesShader->LoadSuccess()
		|| !presentShader->LoadSuccess()||!animationShader->LoadSuccess())
		return;

	currentFrame = 0;
	frameTime = 0.0f;
	useFog = 0;
	useBlur = false;

	sceneTime = 0.0f;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	init = true;
}

Renderer::~Renderer(void)
{
	delete camera;
	delete heightMap;
	delete root;
	delete robot;
	delete animation;
	delete material;

	delete shadowLight;
	delete[] light;

	delete shadowShader;
	delete mainShader;
	delete lightShader;
	delete combineShader;
	delete postProcesShader;
	delete presentShader;
	delete skyboxShader;
	delete animationShader;

	delete sphere;
	delete quad;

	glDeleteTextures(1, &shadowTex);
	glDeleteTextures(1,&bufferColourTex);
	glDeleteTextures(1,&emissionTex);
	glDeleteTextures(1,&bufferNormalTex);
	glDeleteTextures(1,&bufferDepthTex);
	glDeleteTextures(1,&lightDiffuseTex);
	glDeleteTextures(1,&lightSpecularTex);
	glDeleteTextures(1,&fogTex);
	glDeleteTextures(1,&processDepthTex);
	glDeleteTextures(2,processColourTex);

	glDeleteFramebuffers(1,&shadowFBO);
	glDeleteFramebuffers(1,&mainFBO);
	glDeleteFramebuffers(1,&lightFBO);
	glDeleteFramebuffers(1,&combineFBO);
	glDeleteFramebuffers(1,&processFBO);

}

void Renderer::UpdateScene(float dt)
{
	if(!cameraMove)
		camera->UpdateCamera(dt);
	
	root->Update(dt);
	lightObjects->Update(dt);
	
	frameTime -= dt;
	sceneTime += dt;

	//Animation
	while (frameTime < 0.0f) {
		currentFrame = (currentFrame + 1) % animation->GetFrameCount();
		frameTime += 1.0f / animation->GetFrameRate();
	}

	//Camera Move
	if (cameraMove)
	{
		float x=-100*dt;
		float yaw=90;
		Vector3 cameraPos = camera->GetPosition();

		if (cameraPos.x > heightMap->GetHeightmapSize().x * 0.3)
		{
			camera->SetPosition(cameraPos + Vector3(x, 0, 0));
			camera->SetYaw(yaw);
		}
		else
			cameraMove = !cameraMove;
	}

	robotMove += Vector3(0, 0, 20 * dt);

}

void Renderer::AutoCameraMovement()
{
	if (!cameraMove)
	{
		camera->SetPosition(heightMap->GetHeightmapSize() * Vector3(0.95f, 1.2f, 0.50f));
		camera->SetPitch(0.0f);
		camera->SetYaw(90.0f);
	}
	cameraMove = !cameraMove;
}

void Renderer::RenderScene()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	DrawShadows();
	DrawMainScene();
	DrawLights();
	CombineBffers();
	PostProcess();
	PresentScene();
}

void Renderer::GenerateSceneTexture(GLuint& into, bool depth)
{
	glGenTextures(1, &into);
	glBindTexture(GL_TEXTURE_2D, into);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	GLuint format = depth ? GL_DEPTH_COMPONENT24 : GL_RGBA8;
	GLuint type = depth ? GL_DEPTH_COMPONENT : GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, type, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::DrawShadows()
{
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	BindShader(shadowShader);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);


	viewMatrix = Matrix4::BuildViewMatrix(shadowLight->GetPosition(), Vector3(0,0,0));
	projMatrix = Matrix4::Perspective(1.0f, 1000, 1, 150);
	shadowMatrix = projMatrix * viewMatrix;

	UpdateShaderMatrices();
	DrawNode(root,shadowShader);
	
	modelMatrix = robotPosition * Matrix4::Scale(Vector3(30, 30, 30));
	DrawAnimation(robot, animation, robotTextures);
	

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Renderer::DrawMainScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	viewMatrix = camera->BuildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width / (float)height, 45.0f);

	//CubeDraw
	glDepthMask(GL_FALSE);
	BindShader(skyboxShader);
	UpdateShaderMatrices();
	quad->Draw();
	glDepthMask(GL_TRUE);
	modelMatrix.ToIdentity();


	BindShader(mainShader);
	glUniform1i(glGetUniformLocation(mainShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(mainShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(mainShader->GetProgram(), "emissionTex"), 2);

	UpdateShaderMatrices();
	DrawNode(root,mainShader);
	DrawNode(lightObjects, mainShader);
	
	modelMatrix = robotPosition * Matrix4::Scale(Vector3(30, 30, 30));
	DrawAnimation(robot, animation, robotTextures);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Renderer::DrawLights()
{
	glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);


	BindShader(lightShader);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glBlendFunc(GL_ONE, GL_ONE);
	glCullFace(GL_FRONT);
	glDepthFunc(GL_ALWAYS);
	glDepthMask(GL_FALSE);

	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "depthTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferDepthTex);

	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "normTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, bufferNormalTex);

	glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "shadowTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	glUniform3fv(glGetUniformLocation(lightShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());
	glUniform2f(glGetUniformLocation(lightShader->GetProgram(), "pixelSize"), 1.0f / width, 1.0f / height);

	Matrix4 invViewProj = (projMatrix * viewMatrix).Inverse();
	glUniformMatrix4fv(glGetUniformLocation(lightShader->GetProgram(), "inverseProjView"), 1, false, invViewProj.values);

	UpdateShaderMatrices();
	SetShaderLight(*light);
	for (int i = 0; i < LIGHT_NUM; ++i)
	{
		Light& l = light[i];
		SetShaderLight(l);
		sphere->Draw();
	}
	SetShaderLight(*shadowLight);
	sphere->Draw();
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Renderer::CombineBffers()
{
	glBindFramebuffer(GL_FRAMEBUFFER, combineFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	BindShader(combineShader);

	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, bufferColourTex);

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "diffuseLight"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, lightDiffuseTex);

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "specularLight"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, lightSpecularTex);

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "emissionTex"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, emissionTex);

	glUniform1i(glGetUniformLocation(combineShader->GetProgram(), "fogTex"), 4);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, fogTex);

	quad->Draw();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::PostProcess()
{
	glBindFramebuffer(GL_FRAMEBUFFER, processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, processColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(postProcesShader);

	if (useBlur)
		POST_PASSES = 5;
	else
		POST_PASSES = 0;

	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glUniform1i(glGetUniformLocation(postProcesShader->GetProgram(), "sceneTex"), 0);
	glActiveTexture(GL_TEXTURE0);

	for (int i = 0; i < POST_PASSES; i++)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, processColourTex[1], 0);
		glUniform1i(glGetUniformLocation(postProcesShader->GetProgram(), "isVertical"), 0);
		glBindTexture(GL_TEXTURE_2D, processColourTex[0]);
		quad->Draw();

		glUniform1i(glGetUniformLocation(postProcesShader->GetProgram(), "isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, processColourTex[0], 0);
		glBindTexture(GL_TEXTURE_2D, processColourTex[1]);
		quad->Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::PresentScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(presentShader);

	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glBindTexture(GL_TEXTURE_2D, processColourTex[0]);
	glUniform1i(glGetUniformLocation(presentShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	quad->Draw();
}

void Renderer::DrawNode(SceneNode* n, Shader* shader)
{
	if (n->GetMesh())
	{
		Matrix4 model = n->GetWordTransform() * Matrix4::Scale(n->GetModelScale());

		float scale = n->GetTextureSize();
		textureMatrix = Matrix4::Scale(Vector3(scale, scale, 1));
		
		UpdateShaderMatrices();
		glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "modelMatrix"), 1, false, model.values);
		glUniform4fv(glGetUniformLocation(shader->GetProgram(),"nodeColour"), 1, (float*)&n->GetColour());

		if (shader == mainShader)
		{
			glUniform1i(glGetUniformLocation(shader->GetProgram(), "useBump"), 1);
			glUniform1i(glGetUniformLocation(shader->GetProgram(), "useTex"), 1);
			glUniform1i(glGetUniformLocation(shader->GetProgram(), "useFog"), useFog);

			GLuint tex = n->GetTexture();
			if (tex == 0)
				glUniform1i(glGetUniformLocation(shader->GetProgram(), "useTex"), 0);		
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex);


			GLuint norm = n->GetNormalTexture();
			if (tex == 0)
				glUniform1i(glGetUniformLocation(shader->GetProgram(), "useBump"), 0);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, norm);			
			

			GLuint emiss = n->GetEmissionTexture();
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, emiss);
		}



		n->Draw(*this);
	}


	for (vector<SceneNode*>::const_iterator i = n->GetChildIteratorStart(); i != n->GetChildIteratorEnd(); ++i)
		DrawNode(*i, shader);
}

void Renderer::DrawAnimation(Mesh* m, MeshAnimation* meshAnimation, vector<GLuint>& textures)
{
	BindShader(animationShader);
	UpdateShaderMatrices();
	
	
	glUniform1i(glGetUniformLocation(animationShader->GetProgram(), "useBump"), 0);
	glUniform1i(glGetUniformLocation(animationShader->GetProgram(), "useTex"), 1);
	glUniform1i(glGetUniformLocation(animationShader->GetProgram(), "useFog"), useFog);
	
	vector<Matrix4> frameMatrices;
	const Matrix4* invBindPose = m->GetInverseBindPose();
	const Matrix4* BindPose = m->GetBindPose();
	const Matrix4* frameData = meshAnimation->GetJointData(currentFrame);

	for (unsigned int i = 0; i < m->GetJointCount(); ++i)
		frameMatrices.emplace_back(frameData[i] * invBindPose[i]);

	int j = glGetUniformLocation(animationShader->GetProgram(), "joints");
	glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());

	for (int i = 0; i < m->GetSubMeshCount(); ++i)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		m->DrawSubMesh(i);
	}
}
