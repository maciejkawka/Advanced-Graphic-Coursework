#pragma once
#include"Vector4.h"
#include"Vector3.h"

class Light {
public:
	Light(){}
	Light(const Vector3& position, const Vector4& colour, float radius)
	{
		this->position = position;
		this->colour = colour;
		this->radius = radius;
	}

	~Light(void){}

	Vector3 GetPosition() const { return position; }
	void SetPosition(const Vector3& val) { position = val; }

	float GetRadius() const { return radius; }
	void SetRadius(float val) { radius = val; }

	Vector4 GetColour() const { return colour; }
	void SetColour(const Vector4& val) { colour = val; }

	void UpdateLight(float dt)
	{
		Vector3 forward =  Vector3(0, 0, -1);
		Vector3 right =  Vector3(1, 0, 0);

		float speed = 20 * dt;

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_I))
			position += forward * speed;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_K))
			position -= forward * speed;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_J))
			position -= right * speed;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_L))
			position += right * speed;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_O))
			position.y += speed;
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_P))
			position.y -= speed;

	}

protected:
	Vector3 position;
	float radius;
	Vector4 colour;
};