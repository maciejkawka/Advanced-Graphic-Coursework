#pragma once
#include"Matrix4.h"
#include"Vector4.h"
#include"Vector3.h"
#include"Mesh.h"
#include"Shader.h"
#include<vector>

class SceneNode {
public:
	SceneNode(Mesh* m = NULL, Vector4 colour = Vector4(1, 1, 1, 1));
	~SceneNode(void);

	float GetBoundingRadius() const { return boundingRadius; }
	void SetBoundingRadius(float f) { boundingRadius = f; }

	float GetCameraDistance() const { return distanceFromCamera; }
	void SetCameraDistance(float f) { distanceFromCamera = f; }

	void SetTexture(GLuint tex) { texture = tex; }
	GLuint GetTexture() const { return texture; }

	void SetNormalTexture(GLuint norm) { normalTex = norm; }
	GLuint GetNormalTexture() const { return normalTex; }

	void SetTextureSize(float textureSize) { texSize = textureSize; }
	float GetTextureSize() const { return texSize; }

	void SetEmissionTexture(GLuint emis) { emissTex = emis; }
	GLuint GetEmissionTexture() const { return emissTex; }

	static bool CompareByCameraDistance(SceneNode* a, SceneNode* b) {
		return (a->distanceFromCamera < b->distanceFromCamera) ? true : false;
	}

	inline void SetTransform(const Matrix4& matrix) { transform = matrix; }
	inline const Matrix4& GetTransform() const { return transform; }
	inline Matrix4 GetWordTransform() const { return worldTransform; }

	inline Vector4 GetColour() const { return colour; }
	inline void SetColour(Vector4 c) { colour = c; }

	inline Vector3 GetModelScale() const { return modelScale; }
	inline void SetModelScale(Vector3 s) { modelScale = s; }

	inline Mesh* GetMesh() const { return mesh; }
	inline void SetMesh(Mesh* m) { mesh = m; }

	inline Shader* GetShader() const { return shader; }
	inline void SetShader(Shader* s) { shader = s; }


	void AddChild(SceneNode* s);
	void RemoveChild();


	virtual void Update(float dt);
	virtual void Draw(const OGLRenderer& r);

	inline std::vector<SceneNode*>::const_iterator GetChildIteratorStart() { return children.begin(); }
	inline std::vector<SceneNode*>::const_iterator GetChildIteratorEnd() { return children.end(); }
	inline int GetChildrenCount() { return children.size(); }
protected:
	float distanceFromCamera;
	float boundingRadius;
	float texSize;
	GLuint texture;
	GLuint normalTex;
	GLuint emissTex;
	SceneNode* parent;
	Mesh* mesh;
	Shader* shader;
	Matrix4 worldTransform;
	Matrix4 transform;
	Vector3 modelScale;
	Vector4 colour;
	std::vector<SceneNode*> children;

};