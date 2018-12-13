#pragma once
#include "Core/Renderer/Sprite.h"


class Background : public Sprite
{
public:
	Background() {}
	Background(int width, int height);

private:
	int m_StarThreshold;
	int m_ColourThreshold;
};