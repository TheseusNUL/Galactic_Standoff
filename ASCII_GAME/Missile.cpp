#include "Missile.h"
#include "Core/Renderer/ConsoleColours.h"

CHAR_INFO PlayerMissileSprite[] =
{
	{0 ,BACKGROUND_TURQOISE},{0 ,BACKGROUND_TURQOISE},{0 ,BACKGROUND_TURQOISE}
};

CHAR_INFO EnemyMissileSprite0[] =
{
	{0, BACKGROUND_RED},{0, BACKGROUND_RED},{0, BACKGROUND_RED}
};

CHAR_INFO EnemyMissileSprite1[] =
{
	{0, BACKGROUND_YELLOW},{0, BACKGROUND_YELLOW},{0, BACKGROUND_YELLOW}
};

CHAR_INFO EnemyMissileSprite2[] =
{
	{0, BACKGROUND_GREEN},{0, BACKGROUND_GREEN},{0, BACKGROUND_GREEN}
};


Missile::Missile()
	:m_bInitialised(false),
	m_Active(false)
{

}

Missile::~Missile()
{

}


void Missile::Initialise(Vector2& position, Vector2& velocity)
{
	m_Velocity = velocity;

	//Sprite::Initialise(m_Velocity.x > 0.0f ? PlayerMissileSprite : EnemyMissileSprite, Vector2(3, 1));
	
	if (m_Velocity.x > 0.0f)
	{
		Sprite::Initialise(PlayerMissileSprite, Vector2(3, 1));
	}
	else if (m_Velocity.x < -4.5f)
	{
		Sprite::Initialise(EnemyMissileSprite2, Vector2(3, 1));
	}
	else if (m_Velocity.x < -2.5f)
	{
		Sprite::Initialise(EnemyMissileSprite1, Vector2(3, 1));
	}
	else
	{
		Sprite::Initialise(EnemyMissileSprite0, Vector2(3, 1));
	}

	Vector2 startPosition(position.x - (Sprite::GetSize().x / 2), position.y - (Sprite::GetSize().y / 2));
	SetPosition(startPosition);

	m_bInitialised = true;
	m_Active = true;
}

void Missile::Update()
{
	if (!m_bInitialised || !m_Active)
		return;

	SetPosition(Vector2(GetPosition().x + m_Velocity.x, GetPosition().y + m_Velocity.y));

}

void Missile::Render(ASCIIRenderer* pRenderer)
{
	if (!m_bInitialised || !m_Active)
		return;

	Sprite::Render(pRenderer);
}

void Missile::Explode()
{
	m_Active = false;
}
