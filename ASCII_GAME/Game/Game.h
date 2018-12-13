#ifndef _GAME_H_
#define _GAME_H

#include <vector>
#include <fstream>
#include "GameStates.h"
#include "../Core/Renderer/Sprite.h"
#include "../Game/GameObjects/ScoreDisplay.h"
#include "../Player.h"
#include "../Background.h"
#include "../Missile.h"
#include "../EnemyShip.h"
#include "../Core/Renderer/AnimatedSprite.h"
#include "../Core/Maths/Vector3.h"

class ASCIIRenderer;

enum Difficulty
{
	DIFFICULTY_EASY,
	DIFFICULTY_NORMAL,
	DIFFICULTY_HARD,
};

class Game
{
public:
	Game();
	~Game();

	void Initialise();
	void Run();

private:

	//Functions
	void InitialiseRenderer();
	void Update();
	void Render();

	void InitialiseMainMenu();
	void InitialiseHighScoresMenu();
	void InitialiseDifficultySelect();
	void InitialiseGame();
	void InitialiseGameOver();
	void InitialiseYouWin();
	
	void UpdateMainMenu();
	void UpdateDifficultySelect();
	void UpdateGame();
	void UpdateScoreDisplay(ScoreDisplay *display, int value);
	void UpdateGameOver();
	void UpdateYouWin();
	void UpdatePlayerMissiles();
	void UpdateEnemyMissiles();

	void SetDifficulty(Difficulty option);
	void SetEasyDifficulty();
	void SetNormalDifficulty();
	void SetHardifficulty();

	void FireMissile();
	void FireEnemyMissile(EnemyShip &enemy);

	void NewRoundBegins();

	void ReadFromFile();
	void WriteToFile();

	void RenderMainMenu();
	void RenderHighScoresMenu();
	void RenderDifficultySelect();
	void RenderGame();
	void RenderScore();
	void RenderGameOver();
	void RenderYouWin();
	void RenderMissiles(std::vector<Missile*> &missiles);

	//void GetEnemyMissiles(EnemyShip& enemy);

	//Variables
	ASCIIRenderer* m_pRenderer;

	bool m_bInitialised;
	bool m_bExitApp;

	int m_Score;
	int m_EnemyCounter;
	int m_PlayerCounter;

	Player m_Player;

	std::vector<Missile*> m_PlayerMissiles;
	
	std::vector<Missile*> m_EnemyMissiles;

	std::vector<EnemyShip*> m_EnemyShips;

	std::vector<int> m_HighScores;
	std::vector<ScoreDisplay*> m_Scores;

	std::ifstream InFile;
	std::ofstream OutFile;
	
	ScoreDisplay m_ScoreDigit[4];
	
	Background m_Background;

	Sprite m_GameTitle;
	Sprite m_GameOver;
	Sprite m_YouWin;
	Sprite m_YouWinShip;
	Sprite m_SelectDifficulty;
	Sprite m_SelectDifficultyBackground;
	Sprite m_SelectDifficultyTitle;
	Sprite m_Planet;
	Sprite m_PlayerHealth[3];
	Sprite m_Quit;
	Sprite m_Return;

	AnimatedSprite m_SirenTitle;
	AnimatedSprite m_Play;

	E_GAME_STATE m_GameState; //Stores what state the game is in.
	Difficulty m_Difficulty;

};


#endif
