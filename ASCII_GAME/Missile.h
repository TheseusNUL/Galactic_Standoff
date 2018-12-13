#pragma once

#include "Core/Renderer/Sprite.h"

class Missile : public Sprite
{
public:
	Missile();
	~Missile();

	void Initialise(Vector2& position, Vector2& velocity);
	void Update();
	void Render(ASCIIRenderer* pRenderer);
	void Explode();
	void SetVelocity(Vector2 velocity) { m_Velocity = velocity; }

	void SetActive(bool active = true) { m_Active = active; } //sets m_Active to true.
	bool Active() { return m_Active; }

	
private:

	Vector2 m_Velocity;

	bool m_bInitialised;
	bool m_Active;
};
