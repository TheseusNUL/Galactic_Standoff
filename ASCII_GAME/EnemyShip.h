#pragma once

#include "Core/Renderer/AnimatedSprite.h"


class ASCIIRenderer;
class Game;


const int ENEMY_SPRITE_ANIMATION_FRAMES = 2;

class EnemyShip : public AnimatedSprite
{
public:

	EnemyShip();
	~EnemyShip();

	void Initialise(Vector2& position, int fireRate, int sprite, int speed, float initialTime = 0.0f);
	bool Update(float deltaTime);
	void Render(ASCIIRenderer* pRenderer);

	void Explode();

	bool Active() { return m_Active; }
	void SetActive(bool active) { m_Active = active; }

	int GetEnemyType();

private:

	bool FireWeapon();

	bool m_bInitialised;

	float m_AnimationTimer;
	bool m_Active;

	int m_FireRate;

	int m_EnemyType;

	int m_Speed;

};
