#pragma once

#include "Core/Renderer/Sprite.h"

class ASCIIRenderer;

extern const int SCREEN_HEIGHT;

class Player: public Sprite
{
public:

	Player();
	~Player();

	void Initialise(Vector2& position);
	void Update();
	void Render(ASCIIRenderer* pRenderer);

	static int MOVEMENT_SPEED;

private:

	void MoveUp();
	void MoveDown();

	bool UpKeyPressed();
	bool DownKeyPressed();
	bool m_bInitialised;

};